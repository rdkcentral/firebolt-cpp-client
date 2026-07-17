/**
 * Discovery.Watched RCA Harness
 *
 * Minimal harness to measure Discovery.Watched latency and diagnose timeout issues.
 * Configuration via environment variables; supports multiple test modes and output formats.
 */

#include <firebolt/firebolt.h>
#include <firebolt/discovery.h>

#include <chrono>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <memory>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <ctime>

using namespace std::chrono;

// ============================================================================
// Utilities
// ============================================================================

// Generate ISO 8601 timestamp in UTC (e.g., "2026-07-09T10:55:11Z")
static std::string get_iso8601_timestamp() {
    auto now = system_clock::now();
    auto time_t_now = system_clock::to_time_t(now);
    std::tm* gmt = std::gmtime(&time_t_now);
    
    std::ostringstream oss;
    oss << std::put_time(gmt, "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

// ============================================================================
// Configuration
// ============================================================================

struct Config {
    std::string ws_url = "ws://localhost:3474";
    int timeout_ms = 3000;
    int num_calls = 10;
    std::string app_id = "test.app";
    std::string entity_id_prefix = "entity";
    std::string test_mode = "all";  // latency, stress, timeout, all, timeout-trigger
    std::string output_format = "text";  // text, json, csv
    std::string output_file = "-";  // - for stdout
    std::string log_level = "Info";
    bool trigger_timeout = false;  // If true, use unreachable URL to force timeout
    
    static Config from_env() {
        Config c;
        
        if (const char* val = std::getenv("WS_URL")) c.ws_url = val;
        if (const char* val = std::getenv("TIMEOUT_MS")) c.timeout_ms = std::atoi(val);
        if (const char* val = std::getenv("NUM_CALLS")) c.num_calls = std::atoi(val);
        if (const char* val = std::getenv("APP_ID")) c.app_id = val;
        if (const char* val = std::getenv("ENTITY_ID_PREFIX")) c.entity_id_prefix = val;
        if (const char* val = std::getenv("TEST_MODE")) c.test_mode = val;
        if (const char* val = std::getenv("OUTPUT_FORMAT")) c.output_format = val;
        if (const char* val = std::getenv("OUTPUT_FILE")) c.output_file = val;
        if (const char* val = std::getenv("LOG_LEVEL")) c.log_level = val;
        if (const char* val = std::getenv("TRIGGER_TIMEOUT")) c.trigger_timeout = (std::atoi(val) != 0);
        
        return c;
    }
};

// ============================================================================
// Metrics & Results
// ============================================================================

struct CallMetric {
    int sequence;
    std::string entity_id;
    long latency_ms;
    bool success;
    std::string error;
};

struct TestResult {
    std::string name;
    int timeout_ms;
    std::vector<CallMetric> calls;
    
    long min_latency() const {
        if (calls.empty()) return 0;
        auto min = std::min_element(calls.begin(), calls.end(),
            [](const CallMetric& a, const CallMetric& b) { return a.latency_ms < b.latency_ms; });
        return min->latency_ms;
    }
    
    long max_latency() const {
        if (calls.empty()) return 0;
        auto max = std::max_element(calls.begin(), calls.end(),
            [](const CallMetric& a, const CallMetric& b) { return a.latency_ms < b.latency_ms; });
        return max->latency_ms;
    }
    
    double avg_latency() const {
        if (calls.empty()) return 0;
        long sum = 0;
        for (const auto& c : calls) sum += c.latency_ms;
        return static_cast<double>(sum) / calls.size();
    }
    
    int success_count() const {
        int count = 0;
        for (const auto& c : calls) if (c.success) count++;
        return count;
    }
    
    long percentile(int p) const {
        if (calls.empty()) return 0;
        std::vector<long> sorted_latencies;
        for (const auto& c : calls) sorted_latencies.push_back(c.latency_ms);
        std::sort(sorted_latencies.begin(), sorted_latencies.end());
        int idx = (p * sorted_latencies.size()) / 100;
        if (idx >= static_cast<int>(sorted_latencies.size())) idx = sorted_latencies.size() - 1;
        return sorted_latencies[idx];
    }
};

// ============================================================================
// Logging
// ============================================================================

void log_rca(const std::string& msg) {
    std::cerr << "[RCA] " << msg << std::endl;
}

// ============================================================================
// Output Writers
// ============================================================================

class OutputWriter {
public:
    virtual ~OutputWriter() = default;
    virtual void write_report(const Config& cfg, const std::vector<TestResult>& results, const std::string& verdict) = 0;
};

class TextWriter : public OutputWriter {
public:
    void write_report(const Config& cfg, const std::vector<TestResult>& results, const std::string& verdict) override {
        std::ostringstream oss;
        
        oss << "[RCA] Test Report\n";
        oss << "[RCA] ====================\n";
        
        for (const auto& result : results) {
            oss << "[RCA] Test: " << result.name << "\n";
            oss << "[RCA]   Timeout: " << result.timeout_ms << " ms\n";
            oss << "[RCA]   Calls: " << result.calls.size() << "\n";
            oss << "[RCA]   Success: " << result.success_count() << "/" << result.calls.size() << "\n";
            
            if (!result.calls.empty()) {
                oss << "[RCA]   Latency (ms): min=" << result.min_latency()
                    << " avg=" << std::fixed << std::setprecision(1) << result.avg_latency()
                    << " max=" << result.max_latency()
                    << " p50=" << result.percentile(50)
                    << " p95=" << result.percentile(95)
                    << " p99=" << result.percentile(99) << "\n";
            }
            
            oss << "[RCA]\n";
            
            for (const auto& call : result.calls) {
                if (call.success) {
                    oss << "[RCA]   Call " << call.sequence << ": " << call.latency_ms << " ms OK\n";
                } else {
                    oss << "[RCA]   Call " << call.sequence << ": TIMEOUT after " << call.latency_ms << " ms\n";
                }
            }
        }
        
        oss << "[RCA] ====================\n";
        oss << "[RCA] Verdict: " << verdict << "\n";
        
        write_output(oss.str(), cfg.output_file);
    }
    
private:
    void write_output(const std::string& text, const std::string& file) {
        if (file == "-") {
            std::cerr << text << std::endl;
        } else {
            std::ofstream ofs(file, std::ios::app);
            ofs << text << std::endl;
        }
    }
};

class JsonWriter : public OutputWriter {
public:
    void write_report(const Config& cfg, const std::vector<TestResult>& results, const std::string& verdict) override {
        std::ostringstream oss;
        oss << "{\n  \"verdict\": \"" << verdict << "\",\n";
        oss << "  \"tests\": [\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            oss << "    {\n";
            oss << "      \"name\": \"" << result.name << "\",\n";
            oss << "      \"timeout_ms\": " << result.timeout_ms << ",\n";
            oss << "      \"calls\": [\n";
            
            for (size_t j = 0; j < result.calls.size(); ++j) {
                const auto& call = result.calls[j];
                oss << "        {\n";
                oss << "          \"sequence\": " << call.sequence << ",\n";
                oss << "          \"entity_id\": \"" << call.entity_id << "\",\n";
                oss << "          \"latency_ms\": " << call.latency_ms << ",\n";
                oss << "          \"success\": " << (call.success ? "true" : "false") << "\n";
                oss << "        }";
                if (j < result.calls.size() - 1) oss << ",";
                oss << "\n";
            }
            
            oss << "      ]\n";
            oss << "    }";
            if (i < results.size() - 1) oss << ",";
            oss << "\n";
        }
        
        oss << "  ]\n}\n";
        
        write_output(oss.str(), cfg.output_file);
    }
    
private:
    void write_output(const std::string& text, const std::string& file) {
        if (file == "-") {
            std::cerr << text << std::endl;
        } else {
            std::ofstream ofs(file, std::ios::app);
            ofs << text << std::endl;
        }
    }
};

class CsvWriter : public OutputWriter {
public:
    void write_report(const Config& cfg, const std::vector<TestResult>& results, const std::string& verdict) override {
        (void)verdict;  // Verdict is not included in CSV format
        std::ostringstream oss;
        oss << "test_name,timeout_ms,sequence,entity_id,latency_ms,success\n";
        
        for (const auto& result : results) {
            for (const auto& call : result.calls) {
                oss << result.name << ","
                    << result.timeout_ms << ","
                    << call.sequence << ","
                    << call.entity_id << ","
                    << call.latency_ms << ","
                    << (call.success ? "true" : "false") << "\n";
            }
        }
        
        write_output(oss.str(), cfg.output_file);
    }
    
private:
    void write_output(const std::string& text, const std::string& file) {
        if (file == "-") {
            std::cerr << text << std::endl;
        } else {
            std::ofstream ofs(file, std::ios::app);
            ofs << text << std::endl;
        }
    }
};

// ============================================================================
// Test Functions
// ============================================================================

TestResult run_latency_baseline(
    const Config& cfg,
    Firebolt::Discovery::IDiscovery* discovery)
{
    TestResult result;
    result.name = "latency_baseline";
    result.timeout_ms = cfg.timeout_ms;
    
    log_rca("Running: Latency Baseline");
    
    for (int i = 0; i < cfg.num_calls; ++i) {
        std::string entity_id = cfg.app_id + "/entity/" + cfg.entity_id_prefix;
        
        auto call_start = high_resolution_clock::now();
        CallMetric metric;
        metric.sequence = i + 1;
        metric.entity_id = entity_id;
        
        try {
            std::string watched_on = get_iso8601_timestamp();
            auto res = discovery->watched(entity_id, 0.5, false, watched_on, std::nullopt);
            (void)res;  // Result is implicitly checked by not throwing
            
            auto call_end = high_resolution_clock::now();
            metric.latency_ms = duration_cast<milliseconds>(call_end - call_start).count();
            metric.success = true;
            
            log_rca("  Call " + std::to_string(i + 1) + ": " + std::to_string(metric.latency_ms) + " ms OK");
            
        } catch (const std::exception& e) {
            auto call_end = high_resolution_clock::now();
            metric.latency_ms = duration_cast<milliseconds>(call_end - call_start).count();
            metric.success = false;
            metric.error = e.what();
            
            log_rca("  Call " + std::to_string(i + 1) + ": TIMEOUT after " + std::to_string(metric.latency_ms) + " ms");
        }
        
        result.calls.push_back(metric);
    }
    
    return result;
}

TestResult run_stress_test(
    const Config& cfg,
    Firebolt::Discovery::IDiscovery* discovery)
{
    TestResult result;
    result.name = "stress";
    result.timeout_ms = cfg.timeout_ms;
    
    log_rca("Running: Stress Test (varying entity IDs)");
    
    for (int i = 0; i < cfg.num_calls; ++i) {
        std::string entity_id = cfg.app_id + "/entity/" + cfg.entity_id_prefix + "-stress-" + std::to_string(i);
        
        auto call_start = high_resolution_clock::now();
        CallMetric metric;
        metric.sequence = i + 1;
        metric.entity_id = entity_id;
        
        try {
            double progress = 0.25 + (i % 4) * 0.125;
            std::string watched_on = get_iso8601_timestamp();
            auto res = discovery->watched(entity_id, progress, false, watched_on, std::nullopt);
            (void)res;  // Result is implicitly checked by not throwing
            
            auto call_end = high_resolution_clock::now();
            metric.latency_ms = duration_cast<milliseconds>(call_end - call_start).count();
            metric.success = true;
            
            log_rca("  [" + entity_id + "]: " + std::to_string(metric.latency_ms) + " ms");
            
        } catch (const std::exception& e) {
            auto call_end = high_resolution_clock::now();
            metric.latency_ms = duration_cast<milliseconds>(call_end - call_start).count();
            metric.success = false;
            metric.error = e.what();
            
            log_rca("  [" + entity_id + "]: TIMEOUT after " + std::to_string(metric.latency_ms) + " ms");
        }
        
        result.calls.push_back(metric);
    }
    
    return result;
}

std::vector<TestResult> run_timeout_sweep(
    const Config& cfg,
    Firebolt::Discovery::IDiscovery* discovery)
{
    std::vector<TestResult> results;
    std::vector<int> timeout_values = {3000, 5000, 10000};
    
    log_rca("Running: Timeout Sweep");
    
    for (int timeout : timeout_values) {
        log_rca("  Testing with timeout=" + std::to_string(timeout) + "ms");
        
        TestResult result;
        result.name = "timeout_sweep";
        result.timeout_ms = timeout;
        
        for (int i = 0; i < cfg.num_calls; ++i) {
            std::string entity_id = cfg.app_id + "/entity/" + cfg.entity_id_prefix + "-timeout-" + std::to_string(timeout);
            
            auto call_start = high_resolution_clock::now();
            CallMetric metric;
            metric.sequence = i + 1;
            metric.entity_id = entity_id;
            
            try {
                std::string watched_on = get_iso8601_timestamp();
                auto res = discovery->watched(entity_id, 0.5, false, watched_on, std::nullopt);
                (void)res;  // Result is implicitly checked by not throwing
                
                auto call_end = high_resolution_clock::now();
                metric.latency_ms = duration_cast<milliseconds>(call_end - call_start).count();
                metric.success = true;
                
            } catch (const std::exception& e) {
                auto call_end = high_resolution_clock::now();
                metric.latency_ms = duration_cast<milliseconds>(call_end - call_start).count();
                metric.success = false;
                metric.error = e.what();
            }
            
            result.calls.push_back(metric);
        }
        
        results.push_back(result);
    }
    
    return results;
}

// ============================================================================
// Main
// ============================================================================

std::mutex connection_mutex;
std::condition_variable connection_cv;
bool is_connected = false;
Firebolt::Error connection_error;

void on_connection_changed(const bool connected, const Firebolt::Error error) {
    std::unique_lock<std::mutex> lock(connection_mutex);
    is_connected = connected;
    connection_error = error;
    connection_cv.notify_all();
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    
    Config cfg = Config::from_env();
    
    // If timeout trigger is enabled, override URL to unreachable address to force timeout
    if (cfg.trigger_timeout) {
        cfg.ws_url = "ws://192.0.2.1:3474";  // RFC 5737 TEST-NET (unreachable)
        log_rca("[TIMEOUT TRIGGER MODE] Overriding URL to unreachable: " + cfg.ws_url);
    }
    
    log_rca("========================================");
    log_rca("Discovery.Watched RCA Harness");
    log_rca("========================================");
    log_rca("WS_URL=" + cfg.ws_url);
    log_rca("TIMEOUT_MS=" + std::to_string(cfg.timeout_ms));
    log_rca("NUM_CALLS=" + std::to_string(cfg.num_calls));
    log_rca("TEST_MODE=" + cfg.test_mode);
    log_rca("");
    
    // Initialize Firebolt
    log_rca("Connecting to " + cfg.ws_url + "...");
    auto connect_start = high_resolution_clock::now();
    
    try {
        auto& accessor = Firebolt::IFireboltAccessor::Instance();
        
        Firebolt::Config fb_cfg;
        fb_cfg.wsUrl = cfg.ws_url;
        
        auto err = accessor.Connect(fb_cfg, on_connection_changed);
        if (err != Firebolt::Error::None) {
            log_rca("ERROR: Failed to initiate connection: " + std::to_string(static_cast<int>(err)));
            return 2;
        }
        
        // Wait for connection
        {
            std::unique_lock<std::mutex> lock(connection_mutex);
            if (!connection_cv.wait_for(lock, seconds(10), [](){ return is_connected; })) {
                auto connect_end = high_resolution_clock::now();
                auto connect_ms = duration_cast<milliseconds>(connect_end - connect_start).count();
                
                if (cfg.trigger_timeout) {
                    log_rca("[TIMEOUT] Connection failed after " + std::to_string(connect_ms) + " ms");
                    log_rca("[TIMEOUT] Error code: " + std::to_string(static_cast<int>(connection_error)));
                    log_rca("[TIMEOUT] This indicates which component times out on unreachable URL");
                    return 2;
                }
                
                log_rca("ERROR: Connection timeout");
                return 2;
            }
        }
        
        auto connect_end = high_resolution_clock::now();
        auto connect_ms = duration_cast<milliseconds>(connect_end - connect_start).count();
        log_rca("Connected in " + std::to_string(connect_ms) + " ms");
        log_rca("");
        
        auto& discovery = accessor.DiscoveryInterface();
        
        // Run test modes
        std::vector<TestResult> all_results;
        int total_failures = 0;
        
        if (cfg.test_mode == "all" || cfg.test_mode == "latency") {
            auto result = run_latency_baseline(cfg, &discovery);
            all_results.push_back(result);
            total_failures += result.calls.size() - result.success_count();
        }
        
        if (cfg.test_mode == "all" || cfg.test_mode == "stress") {
            auto result = run_stress_test(cfg, &discovery);
            all_results.push_back(result);
            total_failures += result.calls.size() - result.success_count();
        }
        
        if (cfg.test_mode == "all" || cfg.test_mode == "timeout") {
            auto results = run_timeout_sweep(cfg, &discovery);
            for (const auto& r : results) {
                all_results.push_back(r);
                total_failures += r.calls.size() - r.success_count();
            }
        }
        
        log_rca("");
        
        // Determine verdict
        std::string verdict;
        if (total_failures == 0) {
            verdict = "✓ C++ client succeeded reliably. If FireboltNative fails, the issue is in AAMP transport layer.";
        } else {
            verdict = "✗ C++ client experienced timeouts. Issue may be in gateway or downstream service.";
        }
        
        // Write report
        std::unique_ptr<OutputWriter> writer;
        if (cfg.output_format == "json") {
            writer = std::make_unique<JsonWriter>();
        } else if (cfg.output_format == "csv") {
            writer = std::make_unique<CsvWriter>();
        } else {
            writer = std::make_unique<TextWriter>();
        }
        
        writer->write_report(cfg, all_results, verdict);
        
        // Cleanup
        accessor.Disconnect();
        
        return total_failures > 0 ? 1 : 0;
        
    } catch (const std::exception& e) {
        log_rca("ERROR: Exception: " + std::string(e.what()));
        return 3;  // Config/init error
    }
}
