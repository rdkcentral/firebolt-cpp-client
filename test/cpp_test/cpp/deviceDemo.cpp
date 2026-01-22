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


#include "deviceDemo.h"
#include <iostream>
#include <string>
#include <vector>

#include "json_types/jsondata_device_types.h"

#include "outputstream.h"
extern OutputStream gOutput;

using namespace Firebolt;
using namespace Firebolt::Device;

DeviceDemo::DeviceDemo()
    : IFireboltDemo()
{
    methodsFromRpc("Device");
}

void DeviceDemo::runOption(const int index)
{
    std::string key = names_[index];

    outStream_ << "Running Device method: " << key << std::endl;

    if (key == "Device.class")
    {
        Result<DeviceClass> r = Firebolt::IFireboltAccessor::Instance().DeviceInterface().deviceClass();
        if (validateResult(r))
        {
            gOutput << "Device Class: " << stringFromEnum(Firebolt::Device::JsonData::DeviceClassEnum, r.value())
                    << std::endl;
        }
    }
    else if (key == "Device.uptime")
    {
        Result<uint32_t> r = Firebolt::IFireboltAccessor::Instance().DeviceInterface().uptime();
        if (validateResult(r))
        {
            gOutput << "Device Uptime (seconds): " << r.value() << std::endl;
        }
    }
    else if (key == "Device.timeInActiveState")
    {
        Result<uint32_t> r = Firebolt::IFireboltAccessor::Instance().DeviceInterface().timeInActiveState();
        if (validateResult(r))
        {
            gOutput << "Device Time In Active State (seconds): " << r.value() << std::endl;
        }
    }
    else if (key == "Device.chipsetId")
    {
        Result<std::string> r = Firebolt::IFireboltAccessor::Instance().DeviceInterface().chipsetId();
        if (validateResult(r))
        {
            gOutput << "Device Chipset ID: " << r.value() << std::endl;
        }
    }
    else if (key == "Device.uid")
    {
        Result<std::string> r = Firebolt::IFireboltAccessor::Instance().DeviceInterface().uid();
        if (validateResult(r))
        {
            gOutput << "Device UID: " << r.value() << std::endl;
        }
    }
}
