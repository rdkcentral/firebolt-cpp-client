#include <firebolt/firebolt.h>
#include <nlohmann/json.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include <atomic>
#include <cerrno>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <future>
#include <mutex>
#include <optional>
#include <poll.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <utility>
#include <vector>

namespace
{

using namespace std::chrono_literals;
using LifecycleState = Firebolt::Lifecycle::LifecycleState;

constexpr std::chrono::milliseconds kFreezeDuration{1200};
constexpr std::chrono::milliseconds kTimeout{15000};
constexpr std::chrono::milliseconds kGracePeriod{500};
constexpr std::chrono::milliseconds kRequestTimeout{1000};

bool writeAll(int fd, const std::string& data)
{
    const char* current = data.data();
    std::size_t remaining = data.size();
    while (remaining > 0)
    {
        const ssize_t written = ::write(fd, current, remaining);
        if (written < 0 && errno == EINTR)
        {
            continue;
        }
        if (written <= 0)
        {
            return false;
        }
        current += written;
        remaining -= static_cast<std::size_t>(written);
    }
    return true;
}

bool readLine(int fd, std::string& line, std::chrono::milliseconds timeout)
{
    line.clear();
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (true)
    {
        const auto remaining =
            std::chrono::duration_cast<std::chrono::milliseconds>(deadline - std::chrono::steady_clock::now());
        if (remaining <= 0ms)
        {
            return false;
        }

        pollfd descriptor{fd, POLLIN, 0};
        const int ready = ::poll(&descriptor, 1, static_cast<int>(remaining.count()));
        if (ready < 0 && errno == EINTR)
        {
            continue;
        }
        if (ready <= 0)
        {
            return false;
        }

        char character = 0;
        const ssize_t bytesRead = ::read(fd, &character, 1);
        if (bytesRead < 0 && errno == EINTR)
        {
            continue;
        }
        if (bytesRead <= 0)
        {
            return false;
        }
        if (character == '\n')
        {
            return true;
        }
        line.push_back(character);
    }
}

void setPromise(const std::shared_ptr<std::promise<void>>& promise)
{
    try
    {
        promise->set_value();
    }
    catch (const std::future_error&)
    {
    }
}

class HibernationAwareGateway
{
public:
    using Server = websocketpp::server<websocketpp::config::asio>;
    using Handle = websocketpp::connection_hdl;

    HibernationAwareGateway() = default;
    HibernationAwareGateway(const HibernationAwareGateway&) = delete;
    HibernationAwareGateway& operator=(const HibernationAwareGateway&) = delete;

    ~HibernationAwareGateway() { stop(); }

    bool start()
    {
        try
        {
            server_.init_asio();
            server_.clear_access_channels(websocketpp::log::alevel::all);
            server_.clear_error_channels(websocketpp::log::elevel::all);
            server_.set_open_handler(
                [this](Handle handle)
                {
                    std::lock_guard<std::mutex> lock(mutex_);
                    ++connectionCount_;
                    clients_.push_back(handle);
                });
            server_.set_message_handler([this](Handle handle, Server::message_ptr message)
                                        { onMessage(handle, message); });

            websocketpp::lib::asio::ip::tcp::endpoint endpoint(websocketpp::lib::asio::ip::address_v4::loopback(), 0);
            server_.listen(endpoint);
            websocketpp::lib::asio::error_code error;
            port_ = server_.get_local_endpoint(error).port();
            if (error)
            {
                return false;
            }

            server_.start_accept();
            thread_ = std::thread([this]() { server_.run(); });
            started_ = true;
            return true;
        }
        catch (const std::exception&)
        {
            return false;
        }
    }

    void stop()
    {
        if (!started_)
        {
            return;
        }
        started_ = false;
        server_.stop_listening();
        server_.stop();
        if (thread_.joinable())
        {
            thread_.join();
        }
    }

    std::string url() const { return "ws://127.0.0.1:" + std::to_string(port_); }

    bool waitForStateQueries(unsigned count, std::chrono::milliseconds timeout)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        return stateQueryCv_.wait_for(lock, timeout, [&]() { return stateQueryCount_ >= count; });
    }

    void enterHibernation()
    {
        std::vector<Handle> clients;
        {
            std::lock_guard<std::mutex> lock(mutex_);
            hibernating_ = true;
            state_ = "hibernated";
            clients = clients_;
            if (pendingStateRequest_)
            {
                pendingStateRequest_.reset();
                ++droppedMessageCount_;
            }
        }

        for (const Handle& client : clients)
        {
            sendStateChange(client, "active", "hibernated");
        }
    }

    void activate()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        hibernating_ = false;
    }

    unsigned stateQueryCount() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return stateQueryCount_;
    }

    unsigned connectionCount() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return connectionCount_;
    }

    unsigned droppedMessageCount() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return droppedMessageCount_;
    }

private:
    struct PendingStateRequest
    {
        Handle handle;
        nlohmann::json id;
    };

    bool send(Handle handle, const nlohmann::json& message)
    {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (hibernating_)
            {
                ++droppedMessageCount_;
                return false;
            }
        }

        websocketpp::lib::error_code error;
        server_.send(handle, message.dump(), websocketpp::frame::opcode::text, error);
        return !error;
    }

    bool sendStateChange(Handle handle, const std::string& oldState, const std::string& newState)
    {
        return send(handle, {{"jsonrpc", "2.0"},
                             {"method", "Lifecycle2.onStateChanged"},
                             {"params", nlohmann::json::array({{{"oldState", oldState}, {"newState", newState}}})}});
    }

    void onMessage(Handle handle, const Server::message_ptr& message)
    {
        const nlohmann::json request = nlohmann::json::parse(message->get_payload(), nullptr, false);
        if (request.is_discarded() || !request.is_object() || !request.contains("id") || !request.contains("method"))
        {
            return;
        }

        const std::string method = request["method"].get<std::string>();
        nlohmann::json response = {{"jsonrpc", "2.0"}, {"id", request["id"]}};
        if (method == "Lifecycle2.state")
        {
            std::string currentState;
            {
                std::lock_guard<std::mutex> lock(mutex_);
                ++stateQueryCount_;
                if (stateQueryCount_ == 2)
                {
                    pendingStateRequest_ = PendingStateRequest{handle, request["id"]};
                }
                else
                {
                    currentState = state_;
                }
            }
            stateQueryCv_.notify_all();

            if (stateQueryCount() == 2)
            {
                return;
            }
            response["result"] = currentState;
            send(handle, response);
            return;
        }

        response["result"] = nlohmann::json::object();
        send(handle, response);
        if (method == "Lifecycle2.onStateChanged" && request.contains("params") && request["params"].is_object() &&
            request["params"].value("listen", false))
        {
            sendStateChange(handle, "initializing", "active");
        }
    }

    Server server_;
    std::thread thread_;
    bool started_ = false;
    uint16_t port_ = 0;
    mutable std::mutex mutex_;
    std::condition_variable stateQueryCv_;
    std::string state_ = "active";
    bool hibernating_ = false;
    unsigned stateQueryCount_ = 0;
    unsigned droppedMessageCount_ = 0;
    unsigned connectionCount_ = 0;
    std::optional<PendingStateRequest> pendingStateRequest_;
    std::vector<Handle> clients_;
};

[[noreturn]] void runClient(int urlFd, int readyFd, int reportFd)
{
    std::string url;
    if (!readLine(urlFd, url, kTimeout))
    {
        ::_exit(10);
    }

    Firebolt::Config config;
    config.wsUrl = url;
    config.waitTime_ms = static_cast<unsigned>(kRequestTimeout.count());
    config.watchdogCycle_ms = 100;
    config.stopGapThreshold_ms = 500;
    config.resumeGrace_ms = static_cast<unsigned>(kGracePeriod.count());
    config.log.level = Firebolt::LogLevel::Error;

    Firebolt::IFireboltAccessor& client = Firebolt::IFireboltAccessor::Instance();
    auto connected = std::make_shared<std::promise<void>>();
    auto connectedFuture = connected->get_future();
    if (client.Connect(config,
                       [connected](bool isConnected, Firebolt::Error)
                       {
                           if (isConnected)
                           {
                               setPromise(connected);
                           }
                       }) != Firebolt::Error::None ||
        connectedFuture.wait_for(5s) != std::future_status::ready)
    {
        writeAll(reportFd, "CONNECT_FAILED\n");
        client.Disconnect();
        ::_exit(11);
    }

    auto initialActive = std::make_shared<std::promise<void>>();
    auto hibernated = std::make_shared<std::promise<void>>();
    auto initialFuture = initialActive->get_future();
    auto hibernatedFuture = hibernated->get_future();
    std::mutex changesMutex;
    unsigned hibernatedCount = 0;

    auto subscription = client.LifecycleInterface().subscribeOnStateChanged(
        [initialActive, hibernated, &changesMutex, &hibernatedCount](const auto& changes)
        {
            for (const auto& change : changes)
            {
                if (change.oldState == LifecycleState::INITIALIZING && change.newState == LifecycleState::ACTIVE)
                {
                    setPromise(initialActive);
                }
                if (change.newState == LifecycleState::HIBERNATED)
                {
                    {
                        std::lock_guard<std::mutex> lock(changesMutex);
                        ++hibernatedCount;
                    }
                    setPromise(hibernated);
                }
            }
        });
    if (!subscription || initialFuture.wait_for(kTimeout) != std::future_status::ready)
    {
        writeAll(reportFd, "SUBSCRIBE_FAILED\n");
        client.Disconnect();
        ::_exit(12);
    }

    const auto initialState = client.LifecycleInterface().state();
    if (!initialState || *initialState != LifecycleState::ACTIVE)
    {
        writeAll(reportFd, "BASELINE_API_FAILED\n");
        client.Disconnect();
        ::_exit(13);
    }

    std::promise<Firebolt::Error> pendingErrorPromise;
    auto pendingErrorFuture = pendingErrorPromise.get_future();
    std::thread pendingCall(
        [&client, &pendingErrorPromise]()
        {
            const auto result = client.LifecycleInterface().state();
            pendingErrorPromise.set_value(result ? Firebolt::Error::None : result.error());
        });

    if (!writeAll(readyFd, "READY\n"))
    {
        writeAll(reportFd, "READY_SIGNAL_FAILED\n");
        client.Disconnect();
        ::_exit(14);
    }

    if (hibernatedFuture.wait_for(kTimeout) != std::future_status::ready)
    {
        writeAll(reportFd, "RECONCILIATION_TIMEOUT\n");
        client.Disconnect();
        ::_exit(15);
    }

    const auto resumedState = client.LifecycleInterface().state();
    if (pendingErrorFuture.wait_for(kTimeout) != std::future_status::ready)
    {
        writeAll(reportFd, "PENDING_CALL_TIMEOUT\n");
        client.Disconnect();
        ::_exit(16);
    }
    const Firebolt::Error pendingError = pendingErrorFuture.get();
    pendingCall.join();

    unsigned observedHibernatedCount = 0;
    {
        std::lock_guard<std::mutex> lock(changesMutex);
        observedHibernatedCount = hibernatedCount;
    }

    const bool pendingCallTimedOut = pendingError == Firebolt::Error::Timedout;
    const bool resumedApiSucceeded = resumedState && *resumedState == LifecycleState::HIBERNATED;
    client.Disconnect();

    const std::string report = "RESULT " + std::to_string(observedHibernatedCount) + " " +
                               std::to_string(pendingCallTimedOut ? 1 : 0) + " " +
                               std::to_string(resumedApiSucceeded ? 1 : 0) + "\n";
    ::_exit(writeAll(reportFd, report) ? 0 : 17);
}

bool runScenario()
{
    int urlPipe[2] = {-1, -1};
    int readyPipe[2] = {-1, -1};
    int reportPipe[2] = {-1, -1};
    if (::pipe(urlPipe) != 0 || ::pipe(readyPipe) != 0 || ::pipe(reportPipe) != 0)
    {
        std::perror("pipe");
        return false;
    }

    const pid_t child = ::fork();
    if (child < 0)
    {
        std::perror("fork");
        return false;
    }
    if (child == 0)
    {
        ::close(urlPipe[1]);
        ::close(readyPipe[0]);
        ::close(reportPipe[0]);
        runClient(urlPipe[0], readyPipe[1], reportPipe[1]);
    }

    ::close(urlPipe[0]);
    ::close(readyPipe[1]);
    ::close(reportPipe[1]);

    bool passed = false;
    std::string failure;
    int childStatus = 0;
    {
        HibernationAwareGateway gateway;
        std::string ready;
        std::string report;
        if (!gateway.start())
        {
            failure = "mock gateway failed to start";
        }
        else if (!writeAll(urlPipe[1], gateway.url() + "\n"))
        {
            failure = "could not send gateway URL to client";
        }
        else if (!readLine(readyPipe[0], ready, kTimeout) || ready != "READY")
        {
            failure = "client did not connect, subscribe, and start a pending API call";
        }
        else if (!gateway.waitForStateQueries(2, kTimeout))
        {
            failure = "server did not receive the pending API call";
        }
        else if (::kill(child, SIGSTOP) != 0 || ::waitpid(child, &childStatus, WUNTRACED) != child ||
                 !WIFSTOPPED(childStatus))
        {
            failure = "could not stop client process";
        }
        else
        {
            gateway.enterHibernation();
            std::this_thread::sleep_for(kFreezeDuration);
            gateway.activate();
            if (::kill(child, SIGCONT) != 0)
            {
                failure = "could not resume client process";
            }
            else if (!readLine(reportPipe[0], report, kTimeout))
            {
                failure = "client did not report post-resume results";
            }
            else if (report != "RESULT 1 1 1")
            {
                failure = "expected one synthesized hibernated change, one deterministic timeout, and a successful "
                          "post-resume API call; got " +
                          report;
            }
            else if (gateway.stateQueryCount() != 4)
            {
                failure = "expected baseline, pending, reconciliation, and resumed API state queries; got " +
                          std::to_string(gateway.stateQueryCount());
            }
            else if (gateway.droppedMessageCount() != 2)
            {
                failure = "expected the in-flight response and lifecycle event to be dropped without replay; got " +
                          std::to_string(gateway.droppedMessageCount()) + " dropped messages";
            }
            else if (gateway.connectionCount() != 1)
            {
                failure = "expected the client to resume on its original connection; got " +
                          std::to_string(gateway.connectionCount()) + " WebSocket connections";
            }
            else
            {
                passed = true;
            }
        }

        if (!passed)
        {
            ::kill(child, SIGCONT);
            ::kill(child, SIGTERM);
        }
        if (::waitpid(child, &childStatus, 0) != child || !WIFEXITED(childStatus) || WEXITSTATUS(childStatus) != 0)
        {
            passed = false;
            if (failure.empty())
            {
                failure = "client process exited unsuccessfully";
            }
        }
    }

    ::close(urlPipe[1]);
    ::close(readyPipe[0]);
    ::close(reportPipe[0]);

    if (passed)
    {
        std::printf("[       OK ] dropped hibernation messages, state reconciliation, and post-resume API call\n");
    }
    else
    {
        std::printf("[  FAILED  ] %s\n", failure.c_str());
    }
    return passed;
}

} // namespace

int main()
{
    return runScenario() ? 0 : 1;
}