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

Result<ListVoicesResponse> TextToSpeechImpl::listVoices(const std::string& language)
{
    nlohmann::json params;
    params["language"] = language;
    return helper_.get<JsonData::ListVoicesResponse, ListVoicesResponse>("TextToSpeech.listvoices", params);
}

Result<SpeechResponse> TextToSpeechImpl::speak(const std::string& text)
{
    nlohmann::json params;
    params["text"] = text;
    return helper_.get<JsonData::SpeechResponse, SpeechResponse>("TextToSpeech.speak", params);
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
    return subscriptionManager_.subscribe<JsonData::SpeechIdEvent>("TextToSpeech.onPlaybackerror",
                                                                   std::move(notification));
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
