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

Result<TTSEnabled> TextToSpeechImpl::isttsenabled()
{
    return helper_.get<JsonData::TTSEnabled, TTSEnabled>("TextToSpeech.isttsenabled");
}

Result<ListVoicesResponse> TextToSpeechImpl::listvoices(const std::string& language)
{
    nlohmann::json params;
    params["language"] = language;
    return helper_.get<JsonData::ListVoicesResponse, ListVoicesResponse>("TextToSpeech.listvoices", params);
}

Result<TTSConfiguration> TextToSpeechImpl::getttsconfiguration()
{
    return helper_.get<JsonData::TTSConfiguration, TTSConfiguration>("TextToSpeech.getttsconfiguration");
}

Result<SpeechResponse> TextToSpeechImpl::speak(const std::string& text, const std::optional<std::string>& callsign)
{
    nlohmann::json params;
    params["text"] = text;
    if (callsign.has_value())
    {
        params["callsign"] = callsign.value();
    }
    return helper_.get<JsonData::SpeechResponse, SpeechResponse>("TextToSpeech.speak", params);
}

Result<TTSStatusResponse> TextToSpeechImpl::setttsconfiguration(
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

Result<SpeechStateResponse> TextToSpeechImpl::getspeechstate(SpeechId speechId)
{
    nlohmann::json params;
    params["speechid"] = speechId;
    return helper_.get<JsonData::SpeechStateResponse, SpeechStateResponse>("TextToSpeech.getspeechstate", params);
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnWillspeak(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("texttospeech.onWillspeak", std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnSpeechstart(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("texttospeech.onSpeechstart", std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnSpeechpause(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("texttospeech.onSpeechpause", std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnSpeechresume(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("texttospeech.onSpeechresume",
                                                                   std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnSpeechcomplete(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("texttospeech.onSpeechcomplete",
                                                                   std::move(notification));
}

Result<SubscriptionId>
TextToSpeechImpl::subscribeOnSpeechinterrupted(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("texttospeech.onSpeechinterrupted",
                                                                   std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnNetworkerror(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("texttospeech.onNetworkerror",
                                                                   std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnPlaybackerror(std::function<void(const SpeechIdEvent&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("texttospeech.onPlaybackError",
                                                                   std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnTTSstatechanged(std::function<void(const TTSState&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::TTSState>("texttospeech.onTTSstatechanged", std::move(notification));
}

Result<SubscriptionId> TextToSpeechImpl::subscribeOnVoicechanged(std::function<void(const TTSVoice&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::TTSVoice>("texttospeech.onVoicechanged", std::move(notification));
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
