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

#include "discoveryDemo.h"
#include "utils.h"
#include <firebolt/firebolt.h>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

#include "json_types/jsondata_common.h"

using namespace Firebolt;
using namespace Firebolt::Discovery;

DiscoveryDemo::DiscoveryDemo()
    : DemoBase("Discovery")
{
    methods_.push_back("Discovery.watched");
}

void DiscoveryDemo::runOption(const std::string& method)
{
    std::cout << "Running Discovery method: " << method << std::endl;

    if (method == "Discovery.watched")
    {
        std::string entityId = paramFromConsole("entityId", "exampleEntityId");
        std::optional<double> progress = 0.5;
        try
        {
            progress = std::stod(
                paramFromConsole("progress (percentage as (0-0.999) for VOD, number of seconds for live)", "0.5"));
        }
        catch (const std::exception&)
        {
        }
        std::optional<bool> completed = paramFromConsole("completed (true/false)", "false") == "true";
        std::string watchedOn = paramFromConsole("watchedOn (ISO 8601 timestamp)", "2024-01-01T00:00:00Z");

        std::optional<Firebolt::AgePolicy> agePolicyOpt =
            chooseEnumFromList(Firebolt::JsonData::AgePolicyEnum, "Choose an age policy for the watch event:");

        auto r = Firebolt::IFireboltAccessor::Instance().DiscoveryInterface().watched(entityId, progress, completed,
                                                                                      watchedOn, agePolicyOpt);
        if (succeed(r))
        {
            std::cout << "Discovery.watched result: " << std::boolalpha << *r << std::endl;
        }
    }
}
