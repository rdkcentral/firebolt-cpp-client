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

    Result<ListVoicesResponse> listVoices(const std::string& language) const override;
    Result<SpeechResponse> speak(const std::string& text) const override;
    Result<TTSStatusResponse> pause(SpeechId speechId) const override;
    Result<TTSStatusResponse> resume(SpeechId speechId) const override;
    Result<TTSStatusResponse> cancel(SpeechId speechId) const override;
    Result<SpeechStateResponse> getSpeechState(SpeechId speechId) const override;

    Result<SubscriptionId> subscribeOnWillSpeak(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<SubscriptionId> subscribeOnSpeechStart(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<SubscriptionId> subscribeOnSpeechPause(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<SubscriptionId> subscribeOnSpeechResume(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<SubscriptionId> subscribeOnSpeechComplete(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<SubscriptionId> subscribeOnSpeechInterrupted(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<SubscriptionId> subscribeOnNetworkError(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<SubscriptionId> subscribeOnPlaybackError(std::function<void(const SpeechIdEvent&)>&& notification) override;
    Result<void> unsubscribe(SubscriptionId id) override;
    void unsubscribeAll() override;

private:
    Firebolt::Helpers::IHelper& helper_;
    Firebolt::Helpers::SubscriptionManager subscriptionManager_;
};
} // namespace Firebolt::TextToSpeech
