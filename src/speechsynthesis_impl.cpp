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

#include "speechsynthesis_impl.h"
#include "bcp47.h"
#include "json_types/speechsynthesis.h"
#include <firebolt/json_types.h>
#include <firebolt/types.h>

namespace Firebolt::SpeechSynthesis
{
namespace
{
bool isInRange(const std::optional<double>& value, double min, double max)
{
    // Written so that NaN is rejected
    return !value || (*value >= min && *value <= max);
}
} // namespace

SpeechSynthesisImpl::SpeechSynthesisImpl(Firebolt::Helpers::IHelper& helper)
    : helper_(helper),
      subscriptionManager_(helper, this)
{
}

Result<unsigned> SpeechSynthesisImpl::speak(const std::string& text, std::optional<std::string> lang,
                                            std::optional<std::string> voice, std::optional<double> volume,
                                            std::optional<double> rate, std::optional<double> pitch,
                                            std::optional<bool> pii) const
{
    // Firebolt 9: lang is BCP 47, volume 0 to 1, rate 0.1 to 10, pitch 0 to 2 (all inclusive)
    if ((lang && !Bcp47::isValid(*lang)) || !isInRange(volume, 0.0, 1.0) || !isInRange(rate, 0.1, 10.0) ||
        !isInRange(pitch, 0.0, 2.0))
    {
        return Result<unsigned>{Error::InvalidParams};
    }

    nlohmann::json params;
    params["text"] = text;

    if (lang)
    {
        params["lang"] = *lang;
    }
    if (voice)
    {
        params["voice"] = *voice;
    }
    if (volume)
    {
        params["volume"] = *volume;
    }
    if (rate)
    {
        params["rate"] = *rate;
    }
    if (pitch)
    {
        params["pitch"] = *pitch;
    }
    if (pii)
    {
        params["pii"] = *pii;
    }

    return helper_.get<Firebolt::JsonData::StrictUnsigned, unsigned>("SpeechSynthesis.speak", params);
}
Result<std::pmr::vector<Voice>> SpeechSynthesisImpl::voices() const
{
    return helper_.get<JsonData::VoicesResponse, std::pmr::vector<Voice>>("SpeechSynthesis.voices");
}

Result<SubscriptionId>
SpeechSynthesisImpl::subscribeOnVoicesChanged(std::function<void(const std::pmr::vector<Voice>&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::VoicesResponse>("SpeechSynthesis.onVoicesChanged",
                                                                    std::move(notification));
}
Result<void> SpeechSynthesisImpl::cancel(UtteranceId utteranceId) const
{
    nlohmann::json params;
    params["utteranceId"] = utteranceId;
    return helper_.invoke("SpeechSynthesis.cancel", params);
}
Result<void> SpeechSynthesisImpl::pause(UtteranceId utteranceId) const
{
    nlohmann::json params;
    params["utteranceId"] = utteranceId;
    return helper_.invoke("SpeechSynthesis.pause", params);
}
Result<void> SpeechSynthesisImpl::resume(UtteranceId utteranceId) const
{
    nlohmann::json params;
    params["utteranceId"] = utteranceId;
    return helper_.invoke("SpeechSynthesis.resume", params);
}
Result<SubscriptionId>
SpeechSynthesisImpl::subscribeOnUtteranceEvent(std::function<void(const UtteranceEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::UtteranceEventResponse>("SpeechSynthesis.onUtteranceEvent",
                                                                            std::move(notification));
}
Result<void> SpeechSynthesisImpl::unsubscribe(SubscriptionId id)
{
    return subscriptionManager_.unsubscribe(id);
}

void SpeechSynthesisImpl::unsubscribeAll()
{
    subscriptionManager_.unsubscribeAll();
}

} // namespace Firebolt::SpeechSynthesis