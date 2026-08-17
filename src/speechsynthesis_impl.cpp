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
#include "json_types/speechsynthesis.h"
#include <firebolt/json_types.h>
#include <firebolt/types.h>

namespace Firebolt::SpeechSynthesis
{
SpeechSynthesisImpl::SpeechSynthesisImpl(Firebolt::Helpers::IHelper& helper)
    : helper_(helper),
      subscriptionManager_(helper, this)
{
}

Result<unsigned> SpeechSynthesisImpl::speak(const std::string& text, std::optional<std::string> callSign,
                                            std::optional<std::string> language, std::optional<std::string> voice,
                                            std::optional<std::string> volume, std::optional<std::string> rate,
                                            std::optional<std::string> pitch) const
{
    nlohmann::json params;
    params["text"] = text;

    if (callSign)
    {
        params["callSign"] = *callSign;
    }
    if (language)
    {
        params["language"] = *language;
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

    return helper_.get<Firebolt::JSON::Unsigned, unsigned>("SpeechSynthesis.speak", params);
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
Result<void> SpeechSynthesisImpl::cancel(UtteranceId id) const
{
    nlohmann::json params;
    params["id"] = id;
    return helper_.invoke("SpeechSynthesis.cancel", params);
}
Result<void> SpeechSynthesisImpl::pause(UtteranceId id) const
{
    nlohmann::json params;
    params["id"] = id;
    return helper_.invoke("SpeechSynthesis.pause", params);
}
Result<void> SpeechSynthesisImpl::resume(UtteranceId id) const
{
    nlohmann::json params;
    params["id"] = id;
    return helper_.invoke("SpeechSynthesis.resume", params);
}
} // namespace Firebolt::SpeechSynthesis