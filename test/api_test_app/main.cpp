/**
 * Copyright 2025 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "accessibilityDemo.h"
#include "advertisingDemo.h"
#include "deviceDemo.h"
#include "discoveryDemo.h"
#include "displayDemo.h"
#include "lifecycleDemo.h"
#include "localizationDemo.h"
#include "metricsDemo.h"
#include "networkDemo.h"
#include "presentationDemo.h"
#include "statsDemo.h"
#include "texttospeechDemo.h"

#include "utils.h"

#include <cstdlib>
#include <future>
#include <ios>
#include <optional>
#include <unistd.h>
#include <vector>

int main(int argc, char** argv)
{
    auto& appConfig = GetAppConfig();

    Firebolt::LogLevel logLevel = Firebolt::LogLevel::Notice;
    std::string url;
    std::optional<bool> legacyRPCv1;

    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--auto")
        {
            appConfig.autoRun = true;
        }
        else if (std::string(argv[i]) == "--mock")
        {
            url = "ws://127.0.0.1:9998/";
        }
        else if (std::string(argv[i]) == "--platform")
        {
            url = "ws://127.0.0.1:3474/";
        }
        else if (std::string(argv[i]) == "--url")
        {
            if (i + 1 >= argc)
            {
                std::cerr << "Error: --url option requires a URL argument. Use --help to see usage" << std::endl;
                return 1;
            }
            url = argv[++i];
        }
        else if (std::string(argv[i]) == "--legacy")
        {
            legacyRPCv1 = true;
        }
        else if (std::string(argv[i]) == "--rpc-v2")
        {
            legacyRPCv1 = false;
        }
        else if (std::string(argv[i]) == "--dbg")
        {
            logLevel = Firebolt::LogLevel::Debug;
        }
        else if (std::string(argv[i]) == "--help")
        {
            /* clang-format off */
            std::cout << "SYNOPSIS" << std::endl;
            std::cout << "  " << argv[0] << " [<options>]" << std::endl;
            std::cout << std::endl;
            std::cout << "OPTIONS" << std::endl;
            std::cout << "  --auto       Automatically run all methods for all interfaces without user input" << std::endl;
            std::cout << "  --mock       Connect to a local mock server instead of the default Firebolt Demo Service" << std::endl;
            std::cout << "  --platform   Connect to the platform's Firebolt service (default if available)" << std::endl;
            std::cout << "  --url <URL>  Specify a custom URL for the Firebolt service" << std::endl;
            std::cout << "  --legacy     Use legacy communication" << std::endl;
            std::cout << "  --rpc-v2     Use JSON-RPC compliant communication" << std::endl;
            std::cout << "  --dbg        Enable debug logging" << std::endl;
            std::cout << "  --help       Show this help message" << std::endl;
            /* clang-format on */
            return 0;
        }
    }

    if (url.empty())
    {
        const char* fireboltEndpoint = std::getenv("FIREBOLT_ENDPOINT");
        if (fireboltEndpoint)
        {
            url = fireboltEndpoint;
        }
        else
        {
            url = "ws://127.0.0.1:9998/";
        }
    }
    std::cout << "Using firebolt URL: " << url << std::endl;

    Firebolt::Config config;
    config.wsUrl = url;
    config.waitTime_ms = 1000;
    config.log.level = logLevel;
    if (legacyRPCv1.has_value())
    {
        config.legacyRPCv1 = legacyRPCv1.value();
    }

    std::promise<bool> connectionPromise;
    std::once_flag connectionOnce;
    auto future = connectionPromise.get_future();

    /* clang-format off */
    auto result = Firebolt::IFireboltAccessor::Instance().Connect(
        config,
        [&](const bool connected, const Firebolt::Error error)
        {
            std::cout << "Change in connection: connected: " << std::boolalpha << connected << ", error: " << static_cast<int>(error) << std::endl;
            std::call_once(connectionOnce, [&] { connectionPromise.set_value(connected); });
        });
    /* clang-format on */

    if (result != Firebolt::Error::None)
    {
        std::cout << "Connection call failed" << std::endl;
        return 1;
    }
    if (future.wait_for(std::chrono::seconds(2)) == std::future_status::timeout)
    {
        std::cout << "Connection timed out" << std::endl;
        return 1;
    }
    if (!future.get())
    {
        std::cout << "Failed to connect" << std::endl;
        return 1;
    }

    std::vector<std::unique_ptr<DemoBase>> interfaces;

    interfaces.emplace_back(std::make_unique<AccessibilityDemo>());
    interfaces.emplace_back(std::make_unique<AdvertisingDemo>());
    interfaces.emplace_back(std::make_unique<DeviceDemo>());
    interfaces.emplace_back(std::make_unique<DiscoveryDemo>());
    interfaces.emplace_back(std::make_unique<DisplayDemo>());
    interfaces.emplace_back(std::make_unique<LifecycleDemo>());
    interfaces.emplace_back(std::make_unique<LocalizationDemo>());
    interfaces.emplace_back(std::make_unique<MetricsDemo>());
    interfaces.emplace_back(std::make_unique<NetworkDemo>());
    interfaces.emplace_back(std::make_unique<PresentationDemo>());
    interfaces.emplace_back(std::make_unique<StatsDemo>());
    interfaces.emplace_back(std::make_unique<TextToSpeechDemo>());

    if (!isatty(fileno(stdin)))
    {
        appConfig.autoRun = true;
        std::string line;
        while (std::getline(std::cin, line))
        {
            bool found = false;
            for (const auto& interface : interfaces)
            {
                for (const auto& method : interface->methods())
                {
                    if (method == line)
                    {
                        interface->runOption(method);
                        found = true;
                        break;
                    }
                }
                if (found)
                {
                    break;
                }
            }
            if (!found)
            {
                std::cout << "Method not found: " << line << std::endl;
            }
        }
    }
    else if (appConfig.autoRun)
    {
        for (auto& interface : interfaces)
        {
            std::cout << "Auto-running interface: " << interface->name() << std::endl;

            for (auto& method : interface->methods())
            {
                std::cout << "Auto-running method: " << method << std::endl;
                interface->runOption(method);
            }
        }
    }
    else
    {
        std::vector<std::string> interfaceNames;
        for (const auto& interface : interfaces)
        {
            interfaceNames.push_back(interface->name());
        }
        while (true)
        {
            int interfaceIndex = chooseFromList(interfaceNames, "Select an interface to test:");
            if (interfaceIndex == -1)
            {
                break;
            }
            auto& selectedInterface = interfaces[interfaceIndex];
            std::cout << "Selected interface: " << selectedInterface->name() << std::endl;
            std::vector<std::string> methodNames = selectedInterface->methods();
            while (true)
            {
                int methodIndex = chooseFromList(methodNames, "Select a method to run:");
                if (methodIndex == -1)
                {
                    break;
                }
                const std::string& methodName = methodNames[methodIndex];
                std::cout << "Selected method: " << methodName << std::endl;
                selectedInterface->runOption(methodName);
            }
        }
    }

    Firebolt::IFireboltAccessor::Instance().Disconnect();

    return 0;
}
