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
#include "json_types/common.h"
#include <cmath>
#include <firebolt/json_types.h>
#include <string_view>

namespace Firebolt::Discovery
{
namespace
{
// Live content reports seconds, so only the lower bound (and finiteness) can be checked.
bool isValidProgress(const std::optional<double>& progress)
{
    return !progress || (std::isfinite(*progress) && *progress >= 0.0);
}

int toInt(std::string_view digits)
{
    int value = 0;
    for (const char c : digits)
    {
        value = value * 10 + (c - '0');
    }
    return value;
}

// Firebolt 9: "YYYY-MM-DDThh:mm:ss.sssZ" in UTC. Checks the exact shape ('d' is a digit) and the coarse field ranges;
// the platform checks the calendar (days per month, leap years).
bool isValidUtcTimestamp(std::string_view value)
{
    constexpr std::string_view pattern = "dddd-dd-ddTdd:dd:dd.dddZ";
    if (value.size() != pattern.size())
    {
        return false;
    }
    for (size_t i = 0; i < pattern.size(); ++i)
    {
        const bool matches = pattern[i] == 'd' ? (value[i] >= '0' && value[i] <= '9') : value[i] == pattern[i];
        if (!matches)
        {
            return false;
        }
    }

    const int month = toInt(value.substr(5, 2));
    const int day = toInt(value.substr(8, 2));
    return month >= 1 && month <= 12 && day >= 1 && day <= 31 && toInt(value.substr(11, 2)) < 24 &&
           toInt(value.substr(14, 2)) < 60 && toInt(value.substr(17, 2)) < 60;
}

bool isValidWatchedRequest(const std::string& entityId, const std::optional<double>& progress,
                           const std::optional<std::string>& watchedOn)
{
    return !entityId.empty() && isValidProgress(progress) && (!watchedOn || isValidUtcTimestamp(*watchedOn));
}
} // namespace

DiscoveryImpl::DiscoveryImpl(Firebolt::Helpers::IHelper& helper)
    : helper_(helper)
{
}

Result<bool> DiscoveryImpl::watched(const std::string& entityId, std::optional<double> progress,
                                    std::optional<bool> completed, std::optional<std::string> watchedOn,
                                    std::optional<Firebolt::AgePolicy> agePolicy) const
{
    if (!isValidWatchedRequest(entityId, progress, watchedOn))
    {
        return Result<bool>{Error::InvalidParams};
    }

    nlohmann::json parameters;
    parameters["entityId"] = entityId;
    if (progress)
    {
        parameters["progress"] = *progress;
    }
    if (completed)
    {
        parameters["completed"] = *completed;
    }
    if (watchedOn)
    {
        parameters["watchedOn"] = *watchedOn;
    }
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }

    return helper_.get<Firebolt::JSON::Boolean, bool>("Discovery.watched", parameters);
}

Result<void> DiscoveryImpl::watchedV2(const std::string& entityId, std::optional<double> progress,
                                      std::optional<bool> completed, std::optional<std::string> watchedOn,
                                      std::optional<Firebolt::AgePolicy> agePolicy) const
{
    if (!isValidWatchedRequest(entityId, progress, watchedOn))
    {
        return Result<void>{Error::InvalidParams};
    }

    nlohmann::json parameters;
    parameters["entityId"] = entityId;
    if (progress)
    {
        parameters["progress"] = *progress;
    }
    if (completed)
    {
        parameters["completed"] = *completed;
    }
    if (watchedOn)
    {
        parameters["watchedOn"] = *watchedOn;
    }
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }

    return helper_.invoke("Discovery.watched", parameters);
}
} // namespace Firebolt::Discovery
