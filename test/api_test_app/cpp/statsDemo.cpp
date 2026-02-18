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

#include "statsDemo.h"
#include <iostream>
#include <string>
#include <vector>

#include "json_types/jsondata_device_types.h"

#include "outputstream.h"
extern OutputStream gOutput;

using namespace Firebolt;
using namespace Firebolt::Stats;

StatsDemo::StatsDemo()
    : FireboltDemoBase()
{
    methodsFromRpc("Stats");
}

void StatsDemo::runOption(const int index)
{
    std::string key = itemDescriptions_[index].name;

    outStream_ << "Running Stats method: " << key << std::endl;
    if (key == "Stats.memoryUsage")
    {
        Result<MemoryInfo> r = Firebolt::IFireboltAccessor::Instance().StatsInterface().memoryUsage();
        if (validateResult(r))
        {
            MemoryInfo memInfo = r.value();
            gOutput << "User Memory Used: " << memInfo.userMemoryUsed << " / " << memInfo.userMemoryLimit << std::endl;
            gOutput << "GPU Memory Used: " << memInfo.gpuMemoryUsed << " / " << memInfo.gpuMemoryLimit << std::endl;
        }
    }
}
