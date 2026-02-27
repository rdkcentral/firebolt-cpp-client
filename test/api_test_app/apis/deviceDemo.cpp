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

#include "deviceDemo.h"
#include <firebolt/firebolt.h>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

#include "json_types/device.h"

using namespace Firebolt;
using namespace Firebolt::Device;

DeviceDemo::DeviceDemo()
    : DemoBase("Device")
{
    methods_.push_back("Device.chipsetId");
    methods_.push_back("Device.deviceClass");
    methods_.push_back("Device.hdr");
    methods_.push_back("Device.timeInActiveState");
    methods_.push_back("Device.uid");
    methods_.push_back("Device.uptime");
}

void DeviceDemo::runOption(const std::string& method)
{
    std::cout << "Running Device method: " << method << std::endl;

    if (method == "Device.chipsetId")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().DeviceInterface().chipsetId();
        if (succeed(r))
        {
            std::cout << "Device Chipset ID: " << *r << std::endl;
        }
    }
    else if (method == "Device.deviceClass")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().DeviceInterface().deviceClass();
        if (succeed(r))
        {
            std::cout << "Device Class: " << Firebolt::JSON::toString(Firebolt::Device::JsonData::DeviceClassEnum, *r)
                      << std::endl;
        }
    }
    else if (method == "Device.hdr")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().DeviceInterface().hdr();
        if (succeed(r))
        {
            std::cout << "HDR: {" << std::boolalpha << r->hdr10 << ", " << r->hdr10Plus << ", " << r->dolbyVision
                      << ", " << r->hlg << "}" << std::endl;
        }
    }
    else if (method == "Device.timeInActiveState")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().DeviceInterface().timeInActiveState();
        if (succeed(r))
        {
            std::cout << "Device Time In Active State (seconds): " << *r << std::endl;
        }
    }
    else if (method == "Device.uid")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().DeviceInterface().uid();
        if (succeed(r))
        {
            std::cout << "Device UID: " << *r << std::endl;
        }
    }
    else if (method == "Device.uptime")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().DeviceInterface().uptime();
        if (succeed(r))
        {
            std::cout << "Device Uptime (seconds): " << *r << std::endl;
        }
    }
}
