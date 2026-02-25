/**
 * Copyright 2026 Comcast Cable Communications Management, LLC
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

#include "networkDemo.h"
#include <firebolt/firebolt.h>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

using namespace Firebolt;

NetworkDemo::NetworkDemo()
    : DemoBase("Network")
{
    methods_.push_back("Network.connected");
}

void NetworkDemo::runOption(const std::string& method)
{
    std::cout << "Running Network method: " << method << std::endl;

    if (method == "Network.connected")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().NetworkInterface().connected();
        if (succeed(r))
        {
            std::cout << "Network Connected: " << std::boolalpha << *r << std::endl;
        }
    }
}
