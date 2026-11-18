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

#include "texttospeech_impl.h"
#include "bcp47.h"
#include "json_types/texttospeech.h"

#include <algorithm>
#include <cmath>
#include <locale>
#include <sstream>

namespace Firebolt::TextToSpeech
{
namespace
{
// Accepts only an unsigned plain decimal number (digits and at most one '.'; no sign, exponent or spaces) within [min, max].
bool isNumberInRange(const std::string& value, double min, double max)
{
    if (std::count(value.begin(), value.end(), '.') > 1 || value.find_first_not_of("0123456789.") != std::string::npos ||
        value.find_first_of("0123456789") == std::string::npos)
    {
        return false;
    }

    std::istringstream stream(value);
    stream.imbue(std::locale::classic());
    stream >> std::noskipws;
    double number = 0.0;
    stream >> number;
    return !stream.fail() && stream.peek() == std::char_traits<char>::eof() && std::isfinite(number) && number >= min &&
           number <= max;
}

bool isValidSpeakOptions(const std::optional<std::string>& language, const std::optional<std::string>& volume,
                         const std::optional<std::string>& rate, const std::optional<std::string>& pitch)
{
    return (!language || Bcp47::isValid(*language)) && (!volume || isNumberInRange(*volume, 0.0, 1.0)) &&
           (!rate || isNumberInRange(*rate, 0.1, 10.0)) && (!pitch || isNumberInRange(*pitch, 0.0, 2.0));
}
} // namespace

TextToSpeechImpl::TextToSpeechImpl(Firebolt::Helpers::IHelper& helper)
    : helper_(helper),
      subscriptionManager_(helper, this)
{
}

Result<ListVoicesResponse> TextToSpeechImpl::listVoices(const std::string& language) const
{
    if (!Bcp47::isValid(language))
    {
        return Result<ListVoicesResponse>{Error::InvalidParams};
    }

    nlohmann::json params;
    params["language"] = language;
    return helper_.get<JsonData::ListVoicesResponse, ListVoicesResponse>("TextToSpeech.listvoices", params);
}

Result<SpeechResponse> TextToSpeechImpl::speak(const std::string& text, std::optional<std::string> callsign,
                                               std::optional<std::string> language, std::optional<std::string> voice,
                                               std::optional<std::string> volume, std::optional<std::string> rate,
                                               std::optional<std::string> pitch) const
{
    if (!isValidSpeakOptions(language, volume, rate, pitch))
    {
        return Result<SpeechResponse>{Error::InvalidParams};
    }

    nlohmann::json params;
    params["text"] = text;
    if (callsign)
    {
        params["callsign"] = *callsign;
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

    return helper_.get<JsonData::SpeechResponse, SpeechResponse>("TextToSpeech.speak", params);
}

Result<TTSStatusResponse> TextToSpeechImpl::pause(SpeechId speechId) const
{
    nlohmann::json params;
    params["speechid"] = speechId;
    return helper_.get<JsonData::TTSStatusResponse, TTSStatusResponse>("TextToSpeech.pause", params);
}

Result<TTSStatusResponse> TextToSpeechImpl::resume(SpeechId speechId) const
{
    nlohmann::json params;
    params["speechid"] = speechId;
    return helper_.get<JsonData::TTSStatusResponse, TTSStatusResponse>("TextToSpeech.resume", params);
}

Result<TTSStatusResponse> TextToSpeechImpl::cancel(SpeechId speechId) const
{
    nlohmann::json params;
    params["speechid"] = speechId;
    return helper_.get<JsonData::TTSStatusResponse, TTSStatusResponse>("TextToSpeech.cancel", params);
}

Result<SpeechStateResponse> TextToSpeechImpl::getSpeechState(SpeechId speechId) const
{
    nlohmann::json params;
    params["speechid"] = speechId;
    return helper_.get<JsonData::SpeechStateResponse, SpeechStateResponse>("TextToSpeech.getspeechstate", params);
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnWillSpeak(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onWillSpeak", std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnSpeechStart(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onSpeechStart", std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnSpeechPause(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onSpeechPause", std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnSpeechResume(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onSpeechResume",
                                                                   std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnSpeechComplete(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onSpeechComplete",
                                                                   std::move(notification));
}

Result<SubscriptionId>
TextToSpeechImpl::subscribeOnSpeechInterrupted(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onSpeechInterrupted",
                                                                   std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnNetworkError(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onNetworkError",
                                                                   std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnPlaybackError(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onPlaybackError",
                                                                   std::move(notification));
}

Result<void> TextToSpeechImpl::unsubscribe(SubscriptionId id)
{
    return subscriptionManager_.unsubscribe(id);
}

void TextToSpeechImpl::unsubscribeAll()
{
    subscriptionManager_.unsubscribeAll();
}
} // namespace Firebolt::TextToSpeech
