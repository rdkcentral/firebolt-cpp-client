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

#include "metrics_impl.h"
#include "json_types/common.h"
#include "json_types/metrics.h"
#include <firebolt/json_types.h>
#include <stdio.h>

namespace Firebolt::Metrics
{
MetricsImpl::MetricsImpl(Firebolt::Helpers::IHelper& helper)
    : helper_(helper)
{
}

Result<void> MetricsImpl::ready() const
{
    return helper_.invoke("Metrics.ready", nlohmann::json());
}

Result<void> MetricsImpl::signIn() const
{
    return helper_.invoke("Metrics.signIn", nlohmann::json());
}

Result<void> MetricsImpl::signOut() const
{
    return helper_.invoke("Metrics.signOut", nlohmann::json());
}

Result<void> MetricsImpl::startContent(const std::optional<std::string>& entityId,
                                       const std::optional<Firebolt::AgePolicy> agePolicy) const
{
    nlohmann::json parameters;
    if (entityId)
    {
        parameters["entityId"] = *entityId;
    }
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }
    printf("Test coverity workflow %s\n");
    return helper_.invoke("Metrics.startContent", parameters);
}

Result<void> MetricsImpl::stopContent(const std::optional<std::string>& entityId,
                                      const std::optional<Firebolt::AgePolicy> agePolicy) const
{
    nlohmann::json parameters;
    if (entityId)
    {
        parameters["entityId"] = *entityId;
    }
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }
    return helper_.invoke("Metrics.stopContent", parameters);
}

Result<void> MetricsImpl::page(const std::string& pageId, const std::optional<Firebolt::AgePolicy>& agePolicy) const
{
    nlohmann::json parameters;
    parameters["pageId"] = pageId;
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }
    return helper_.invoke("Metrics.page", parameters);
}

Result<void> MetricsImpl::error(const ErrorType type, const std::string& code, const std::string& description,
                                const bool visible, const std::optional<std::map<std::string, std::string>>& parameters,
                                const std::optional<Firebolt::AgePolicy>& agePolicy) const
{
    nlohmann::json jsonParameters;
    jsonParameters["type"] = Firebolt::JSON::toString(JsonData::ErrorTypeEnum, type);
    jsonParameters["code"] = code;
    jsonParameters["description"] = description;
    jsonParameters["visible"] = visible;
    if (parameters)
    {
        nlohmann::json paramString = nlohmann::json::object();
        for (const auto& param : *parameters)
        {
            paramString[param.first] = param.second;
        }
        jsonParameters["parameters"] = paramString;
    }
    if (agePolicy)
    {
        jsonParameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }
    return helper_.invoke("Metrics.error", jsonParameters);
}

Result<void> MetricsImpl::mediaLoadStart(const std::string& entityId,
                                         const std::optional<Firebolt::AgePolicy>& agePolicy) const
{
    nlohmann::json parameters;
    parameters["entityId"] = entityId;
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }
    return helper_.invoke("Metrics.mediaLoadStart", parameters);
}

Result<void> MetricsImpl::mediaPlay(const std::string& entityId, const std::optional<Firebolt::AgePolicy>& agePolicy) const
{
    nlohmann::json parameters;
    parameters["entityId"] = entityId;
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }
    return helper_.invoke("Metrics.mediaPlay", parameters);
}

Result<void> MetricsImpl::mediaPlaying(const std::string& entityId,
                                       const std::optional<Firebolt::AgePolicy>& agePolicy) const
{
    nlohmann::json parameters;
    parameters["entityId"] = entityId;
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }
    return helper_.invoke("Metrics.mediaPlaying", parameters);
}

Result<void> MetricsImpl::mediaPause(const std::string& entityId, const std::optional<Firebolt::AgePolicy>& agePolicy) const
{
    nlohmann::json parameters;
    parameters["entityId"] = entityId;
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }
    return helper_.invoke("Metrics.mediaPause", parameters);
}

Result<void> MetricsImpl::mediaWaiting(const std::string& entityId,
                                       const std::optional<Firebolt::AgePolicy>& agePolicy) const
{
    nlohmann::json parameters;
    parameters["entityId"] = entityId;
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }
    return helper_.invoke("Metrics.mediaWaiting", parameters);
}

Result<void> MetricsImpl::mediaSeeking(const std::string& entityId, const double target,
                                       const std::optional<Firebolt::AgePolicy>& agePolicy) const
{
    nlohmann::json parameters;
    parameters["entityId"] = entityId;
    parameters["target"] = target;
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }
    return helper_.invoke("Metrics.mediaSeeking", parameters);
}

Result<void> MetricsImpl::mediaSeeked(const std::string& entityId, const double position,
                                      const std::optional<Firebolt::AgePolicy>& agePolicy) const
{
    nlohmann::json parameters;
    parameters["entityId"] = entityId;
    parameters["position"] = position;
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }
    return helper_.invoke("Metrics.mediaSeeked", parameters);
}

Result<void> MetricsImpl::mediaRateChanged(const std::string& entityId, const double rate,
                                           const std::optional<Firebolt::AgePolicy>& agePolicy) const
{
    nlohmann::json parameters;
    parameters["entityId"] = entityId;
    parameters["rate"] = rate;
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }
    return helper_.invoke("Metrics.mediaRateChanged", parameters);
}

Result<void> MetricsImpl::mediaRenditionChanged(const std::string& entityId, const unsigned bitrate, const unsigned width,
                                                const unsigned height, const std::optional<std::string>& profile,
                                                const std::optional<Firebolt::AgePolicy>& agePolicy) const
{
    nlohmann::json parameters;
    parameters["entityId"] = entityId;
    parameters["bitrate"] = bitrate;
    parameters["width"] = width;
    parameters["height"] = height;
    if (profile)
    {
        parameters["profile"] = *profile;
    }
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }
    return helper_.invoke("Metrics.mediaRenditionChanged", parameters);
}

Result<void> MetricsImpl::mediaEnded(const std::string& entityId, const std::optional<Firebolt::AgePolicy>& agePolicy) const
{
    nlohmann::json parameters;
    parameters["entityId"] = entityId;
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }
    return helper_.invoke("Metrics.mediaEnded", parameters);
}

Result<void> MetricsImpl::event(const std::string& schema, const std::string& data,
                                const std::optional<Firebolt::AgePolicy>& agePolicy) const
{
    nlohmann::json parameters;
    parameters["schema"] = schema;
    parameters["data"] = data;
    if (agePolicy)
    {
        parameters["agePolicy"] = Firebolt::JSON::toString(Firebolt::JsonData::AgePolicyEnum, *agePolicy);
    }
    return helper_.invoke("Metrics.event", parameters);
}

Result<void> MetricsImpl::appInfo(const std::string& build) const
{
    nlohmann::json parameters;
    parameters["build"] = build;
    return helper_.invoke("Metrics.appInfo", parameters);
}

} // namespace Firebolt::Metrics
