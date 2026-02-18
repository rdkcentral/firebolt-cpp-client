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
#include <iostream>
#include <string>
#include <vector>

#include "json_types/jsondata_discovery.h"

#include "outputstream.h"
extern OutputStream gOutput;

using namespace Firebolt;
using namespace Firebolt::Discovery;

DiscoveryDemo::DiscoveryDemo()
    : FireboltDemoBase()
{
    methodsFromRpc("Discovery");
}

void DiscoveryDemo::runOption(const int index)
{
    std::string key = itemDescriptions_[index].name;

    outStream_ << "Running Discovery method: " << key << std::endl;

    if (key == "Discovery.watched")
    {
        std::string entityId;
        paramFromConsole("entityId", "exampleEntityId", entityId);

        std::string progressStr;
        paramFromConsole("progress (percentage as (0-0.999) for VOD, number of seconds for live)", "0.5", progressStr);
        std::optional<float> progress = std::stof(progressStr);

        std::string completedStr;
        paramFromConsole("completed (true/false)", "false", completedStr);
        std::optional<bool> completed = completedStr == "true";

        std::string watchedOn;
        paramFromConsole("watchedOn (ISO 8601 timestamp)", "2024-01-01T00:00:00Z", watchedOn);

        AgePolicy agePolicy = chooseEnumFromList(Firebolt::Discovery::JsonData::AgePolicyEnum,
                                                 "Choose an age policy for the watch event:");
        std::optional<AgePolicy> agePolicyOpt = agePolicy;

        Result<bool> r = Firebolt::IFireboltAccessor::Instance().DiscoveryInterface().watched(entityId, progress,
                                                                                              completed, watchedOn,
                                                                                              agePolicyOpt);
        if (validateResult(r))
        {
            gOutput << "Discovery.watched result: " << (r.value() ? "true" : "false") << std::endl;
        }
    }
    {
    }
}
