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

#pragma once

#include "firebolt/texttospeech.h"
#include <firebolt/helpers.h>

namespace Firebolt::TextToSpeech
{
class TextToSpeechImpl : public ITextToSpeech
{
public:
    explicit TextToSpeechImpl(Firebolt::Helpers::IHelper& helper);
    TextToSpeechImpl(const TextToSpeechImpl&) = delete;
    TextToSpeechImpl& operator=(const TextToSpeechImpl&) = delete;

    ~TextToSpeechImpl() override = default;

    // Methods
    Result<TTSEnabled> isEnabled() override;
    Result<ListVoicesResponse> listVoices(const std::string& language) override;
    Result<TTSConfiguration> getConfiguration() override;
    Result<TTSStatusResponse>
    setConfiguration(const std::optional<std::string>& ttsEndpoint, const std::optional<std::string>& ttsEndpointSecured,
                     const std::optional<std::string>& language, const std::optional<std::string>& voice,
                     const std::optional<int32_t>& volume, const std::optional<int32_t>& primVolDuckPercent,
                     const std::optional<int32_t>& rate, const std::optional<SpeechRate>& speechRate,
                     const std::optional<FallbackText>& fallbackText) override;
    Result<SpeechResponse> speak(const std::string& text, const std::optional<std::string>& callSign) override;
    Result<TTSStatusResponse> pause(SpeechId speechId) override;
    Result<TTSStatusResponse> resume(SpeechId speechId) override;
    Result<TTSStatusResponse> cancel(SpeechId speechId) override;
    Result<SpeechStateResponse> getSpeechState(SpeechId speechId) override;

    // Events
    Result<SubscriptionId> subscribeOnWillSpeak(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<SubscriptionId> subscribeOnSpeechStart(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<SubscriptionId> subscribeOnSpeechPause(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<SubscriptionId> subscribeOnSpeechResume(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<SubscriptionId> subscribeOnSpeechComplete(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<SubscriptionId> subscribeOnSpeechInterrupted(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<SubscriptionId> subscribeOnNetworkError(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<SubscriptionId> subscribeOnPlaybackError(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<SubscriptionId> subscribeOnTTSStateChanged(std::function<void(const TTSState&)>&& notification) override;
    Result<SubscriptionId> subscribeOnVoiceChanged(std::function<void(const TTSVoice&)>&& notification) override;
    Result<void> unsubscribe(SubscriptionId id) override;
    void unsubscribeAll() override;

private:
    Firebolt::Helpers::IHelper& helper_;
    Firebolt::Helpers::SubscriptionManager subscriptionManager_;
};
} // namespace Firebolt::TextToSpeech
