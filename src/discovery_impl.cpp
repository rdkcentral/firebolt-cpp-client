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

#include "discovery_impl.h"
#include "jsondata_discovery.h"
#include <firebolt/json_types.h>

namespace Firebolt::Discovery
{
DiscoveryImpl::DiscoveryImpl(Firebolt::Helpers::IHelper& helper)
    : helper_(helper)
{
}

Result<bool> DiscoveryImpl::watched(const std::string& entityId, std::optional<float> progress,
                                    std::optional<bool> completed, std::optional<std::string> watchedOn,
                                    std::optional<AgePolicy> agePolicy) const
{
    nlohmann::json parameters;
    parameters["entityId"] = entityId;
    if (progress.has_value())
    {
        parameters["progress"] = progress.value();
    }
    if (completed.has_value())
    {
        parameters["completed"] = completed.value();
    }
    if (watchedOn.has_value())
    {
        parameters["watchedOn"] = watchedOn.value();
    }
    if (agePolicy.has_value())
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(JsonData::AgePolicyEnum, agePolicy.value());
    }

    Result<bool> result = helper_.get<Firebolt::JSON::Boolean, bool>("Discovery.watched", parameters);
    if (!result)
    {
        return Result<bool>{result.error()};
    }
    return Result<bool>{result.value()};
}
} // namespace Firebolt::Discovery
