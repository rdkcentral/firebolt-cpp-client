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
#include <firebolt/firebolt.h>
#include <iostream>
#include <string>
#include <vector>

using namespace Firebolt;

StatsDemo::StatsDemo()
    : DemoBase("Stats")
{
    methods_.push_back("Stats.memoryUsage");
}

void StatsDemo::runOption(const std::string& method)
{
    std::cout << "Running Stats method: " << method << std::endl;
    if (method == "Stats.memoryUsage")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().StatsInterface().memoryUsage();
        if (succeed(r))
        {
            std::cout << "User Memory Used: " << r->userMemoryUsed << " / " << r->userMemoryLimit << std::endl;
            std::cout << "GPU Memory Used: " << r->gpuMemoryUsed << " / " << r->gpuMemoryLimit << std::endl;
        }
    }
}
