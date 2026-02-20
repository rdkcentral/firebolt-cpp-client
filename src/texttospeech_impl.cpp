/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2025 Sky UK
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
 */

#include "texttospeech_impl.h"
#include "jsondata_texttospeech_types.h"

namespace Firebolt::TextToSpeech
{
TextToSpeechImpl::TextToSpeechImpl(Firebolt::Helpers::IHelper& helper)
    : helper_(helper),
      subscriptionManager_(helper, this)
{
}

Result<TTSEnabled> TextToSpeechImpl::isEnabled()
{
    return helper_.get<JsonData::TTSEnabled, TTSEnabled>("TextToSpeech.isttsenabled");
}

Result<ListVoicesResponse> TextToSpeechImpl::listVoices(const std::string& language)
{
    nlohmann::json params;
    params["language"] = language;
    return helper_.get<JsonData::ListVoicesResponse, ListVoicesResponse>("TextToSpeech.listvoices", params);
}

Result<TTSConfiguration> TextToSpeechImpl::getConfiguration()
{
    return helper_.get<JsonData::TTSConfiguration, TTSConfiguration>("TextToSpeech.getttsconfiguration");
}

Result<SpeechResponse> TextToSpeechImpl::speak(const std::string& text, const std::optional<std::string>& callSign)
{
    nlohmann::json params;
    params["text"] = text;
    if (callSign.has_value())
    {
        params["callsign"] = callSign.value();
    }
    return helper_.get<JsonData::SpeechResponse, SpeechResponse>("TextToSpeech.speak", params);
}

Result<TTSStatusResponse> TextToSpeechImpl::setConfiguration(
    const std::optional<std::string>& ttsEndpoint, const std::optional<std::string>& ttsEndpointSecured,
    const std::optional<std::string>& language, const std::optional<std::string>& voice,
    const std::optional<int32_t>& volume, const std::optional<int32_t>& primVolDuckPercent,
    const std::optional<int32_t>& rate, const std::optional<SpeechRate>& speechRate,
    const std::optional<FallbackText>& fallbackText)
{
    nlohmann::json params;
    if (ttsEndpoint.has_value())
    {
        params["ttsendpoint"] = ttsEndpoint.value();
    }
    if (ttsEndpointSecured.has_value())
    {
        params["ttsendpointsecured"] = ttsEndpointSecured.value();
    }
    if (language.has_value())
    {
        params["language"] = language.value();
    }
    if (voice.has_value())
    {
        params["voice"] = voice.value();
    }
    if (volume.has_value())
    {
        params["volume"] = volume.value();
    }
    if (primVolDuckPercent.has_value())
    {
        params["primvolduckpercent"] = primVolDuckPercent.value();
    }
    if (rate.has_value())
    {
        params["rate"] = rate.value();
    }
    if (speechRate.has_value())
    {
        params["speechrate"] = Firebolt::JSON::toString(JsonData::SpeechRateEnum, speechRate.value());
    }
    if (fallbackText.has_value())
    {
        if (fallbackText->scenario.has_value())
        {
            params["fallbacktext"]["scenario"] = fallbackText->scenario.value();
        }
        if (fallbackText->value.has_value())
        {
            params["fallbacktext"]["value"] = fallbackText->value.value();
        }
    }
    return helper_.get<JsonData::TTSStatusResponse, TTSStatusResponse>("TextToSpeech.setttsconfiguration", params);
}

Result<TTSStatusResponse> TextToSpeechImpl::pause(SpeechId speechId)
{
    nlohmann::json params;
    params["speechid"] = speechId;
    return helper_.get<JsonData::TTSStatusResponse, TTSStatusResponse>("TextToSpeech.pause", params);
}

Result<TTSStatusResponse> TextToSpeechImpl::resume(SpeechId speechId)
{
    nlohmann::json params;
    params["speechid"] = speechId;
    return helper_.get<JsonData::TTSStatusResponse, TTSStatusResponse>("TextToSpeech.resume", params);
}

Result<TTSStatusResponse> TextToSpeechImpl::cancel(SpeechId speechId)
{
    nlohmann::json params;
    params["speechid"] = speechId;
    return helper_.get<JsonData::TTSStatusResponse, TTSStatusResponse>("TextToSpeech.cancel", params);
}

Result<SpeechStateResponse> TextToSpeechImpl::getSpeechState(SpeechId speechId)
{
    nlohmann::json params;
    params["speechid"] = speechId;
    return helper_.get<JsonData::SpeechStateResponse, SpeechStateResponse>("TextToSpeech.getspeechstate", params);
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnWillSpeak(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onWillspeak", std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnSpeechStart(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onSpeechstart", std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnSpeechPause(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onSpeechpause", std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnSpeechResume(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onSpeechresume",
                                                                   std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnSpeechComplete(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onSpeechcomplete",
                                                                   std::move(notification));
}

Result<SubscriptionId>
TextToSpeechImpl::subscribeOnSpeechInterrupted(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onSpeechinterrupted",
                                                                   std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnNetworkError(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onNetworkerror",
                                                                   std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnPlaybackError(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onPlaybackError",
                                                                   std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnTTSStateChanged(std::function<void(const TTSState&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::TTSState>("TextToSpeech.onTTSstatechanged", std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnVoiceChanged(std::function<void(const TTSVoice&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::TTSVoice>("TextToSpeech.onVoicechanged", std::move(notification));
}

Result<void> TextToSpeechImpl::unsubscribe(SubscriptionId id)
{
    return subscriptionManager_.unsubscribe(id);
}

void TextToSpeechImpl::unsubscribeAll()
{
    return subscriptionManager_.unsubscribeAll();
}
} // namespace Firebolt::TextToSpeech
