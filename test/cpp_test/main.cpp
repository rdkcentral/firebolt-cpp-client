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

#include <atomic>
#include <chrono>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <thread>
#include <unistd.h>

#include "cpp/chooseInterface.h"

#include "cpp/outputstream.h"

OutputStream gOutput = OutputStream();

std::atomic<bool> gConnected{false};
bool gAutoRun = false;
std::vector<FireboltDemoBase*> interfaces;

void connectionChanged(const bool connected, const Firebolt::Error error)
{
    std::cout << "Change in connection: connected: " << connected << " error: " << static_cast<int>(error) << std::endl;
    gConnected.store(connected, std::memory_order_release);
}

void createFireboltInstance(const std::string& url)
{
    Firebolt::Config config;
    config.wsUrl = url;
    config.waitTime_ms = 1000;
    config.log.level = Firebolt::LogLevel::Debug;

    gConnected = false;
    Firebolt::IFireboltAccessor::Instance().Connect(config, connectionChanged);
}

void destroyFireboltInstance()
{
    Firebolt::IFireboltAccessor::Instance().Disconnect();
}

bool waitOnConnectionReady()
{
    uint32_t waiting = 10000;
    static constexpr uint32_t SLEEPSLOT_TIME = 100;

    // Right, a wait till connection is closed is requested..
    while ((waiting > 0) && !gConnected.load(std::memory_order_acquire))
    {

        uint32_t sleepSlot = (waiting > SLEEPSLOT_TIME ? SLEEPSLOT_TIME : waiting);
        // Right, lets sleep in slices of 100 ms
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepSlot));
        waiting -= sleepSlot;
    }
    return gConnected.load(std::memory_order_acquire);
}

int main(int argc, char** argv)
{
    std::string url = "";

    // check args for -auto option
    for (int i = 1; i < argc; ++i)
    {
        std::cout << "Arg: " << argv[i] << std::endl;
        if (std::string(argv[i]) == "-auto")
        {
            gAutoRun = true;
        }
        else if (std::string(argv[i]) == "-mock")
        {
            url = "ws://127.0.0.1:9998"; // Default URL for local mock server
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
            url = "ws://127.0.0.1:3474"; // Default URL for RDK Central's Firebolt Demo Service
        }
        std::cout << "-----Using firebolt URL: " << url << std::endl;
    }

    createFireboltInstance(url);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    if (!waitOnConnectionReady())
    {
        std::cout << "Test not able to connect with server..." << std::endl;
        return -1;
    }

    ChooseInterface chooseInterface;

    if (gAutoRun)
    {
        chooseInterface.autoRun();
    }
    else
    {
        for (;;)
        {
            int interfaceIndex = chooseInterface.chooseOption();

            if (interfaceIndex == -1)
            {
                break; // Exit the program
            }

            chooseInterface.runOption(interfaceIndex);
        }
    }

    destroyFireboltInstance();

    return 0;
}
