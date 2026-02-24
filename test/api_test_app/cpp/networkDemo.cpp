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
#include <iostream>
#include <string>
#include <vector>

#include "json_types/jsondata_device_types.h"

using namespace Firebolt;
using namespace Firebolt::Device;

NetworkDemo::NetworkDemo()
    : FireboltDemoBase()
{
    methodsFromRpc("Network");
}

void NetworkDemo::runOption(const int index)
{
    std::string key = itemDescriptions_[index].name;

    std::cout << "Running Network method: " << key << std::endl;

    if (key == "Network.connected")
    {
        Result<bool> r = Firebolt::IFireboltAccessor::Instance().NetworkInterface().connected();
        if (validateResult(r))
        {
            std::cout << "Network Connected: " << (r.value() ? "true" : "false") << std::endl;
        }
    }
}
