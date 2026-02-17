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

#include "advertisingDemo.h"
#include <iostream>
#include <string>
#include <vector>

#include "outputstream.h"
extern OutputStream gOutput;

// #include "json_types/jsondata_device_types.h"

using namespace Firebolt;
using namespace Firebolt::Advertising;

AdvertisingDemo::AdvertisingDemo()
    : IFireboltDemo()
{
    methodsFromRpc("Advertising");
}

void AdvertisingDemo::runOption(const int index)
{
    std::string key = itemDescriptions_[index].name;

    if (key == "Advertising.advertisingId")
    {
        Result<Ifa> result = Firebolt::IFireboltAccessor::Instance().AdvertisingInterface().advertisingId();
        if (validateResult(result))
        {
            Ifa ifa = result.value();
            gOutput << "IFA: " << ifa.ifa << std::endl;
            gOutput << "IFA Type: " << ifa.ifa_type << std::endl;
            gOutput << "LMT: " << ifa.lmt << std::endl;
        }
    }
}
