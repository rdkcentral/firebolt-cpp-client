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

#include <firebolt/types.h>

#include <functional>
#include <string>
#include <vector>

namespace Firebolt::TextToSpeech
{
// Enums
enum class SpeechRate
{
    SLOW,
    MEDIUM,
    FAST,
    FASTER,
    FASTEST
};

enum class SpeechState
{
    PENDING = 0,
    IN_PROGRESS = 1,
    PAUSED = 2,
    NOT_FOUND = 3,
};

// Types
using TTSStatus = int32_t;
using SpeechId = int32_t;

struct TTSStatusResponse
{
    TTSStatus ttsStatus;
    bool success;
};

struct SpeechIdEvent
{
    SpeechId speechId;
};

struct ListVoicesResponse
{
    TTSStatus ttsStatus;
    std::vector<std::string> voices;
};

struct SpeechResponse
{
    SpeechId speechId;
    TTSStatus ttsStatus;
    bool success;
};

struct SpeechStateResponse
{
    SpeechState speechState;
    TTSStatus ttsStatus;
    bool success;
};

class ITextToSpeech
{
public:
    virtual ~ITextToSpeech() = default;

    /**
     * @brief Get the list of Text to speech voices supported by the platform
     *
     * @param[in] language : Request language as a BCP 47 locale tag (for example, "en-US")
     *
     * @retval The list of voices supported for the language
     */
    virtual Result<ListVoicesResponse> listVoices(const std::string& language) const = 0;

    /**
     * @brief Speak the uttered text using the TTS engine
     *
     * @param[in] text     : String to be converted to Audio for speech
     *
     * @retval Result for Speak
     */
    virtual Result<SpeechResponse> speak(const std::string& text) const = 0;

    /**
     * @brief Pauses the speech for given speech id
     *
     * @param[in] speechId : Identifier for the speech call
     *
     * @retval Result for Pause
     */
    virtual Result<TTSStatusResponse> pause(SpeechId speechId) const = 0;

    /**
     * @brief Resumes the speech for given speech id
     *
     * @param[in] speechId : Identifier for the speech call
     *
     * @retval Result for Resume
     */
    virtual Result<TTSStatusResponse> resume(SpeechId speechId) const = 0;

    /**
     * @brief Cancels the speech for given speech id
     *
     * @param[in] speechId : Identifier for the speech call
     *
     * @retval Result for cancel
     */
    virtual Result<TTSStatusResponse> cancel(SpeechId speechId) const = 0;

    /**
     * @brief Returns the current state of the speech request.
     *
     * @param[in] speechId : Identifier for the speech call
     *
     * @retval Result for speech state
     */
    virtual Result<SpeechStateResponse> getSpeechState(SpeechId speechId) const = 0;

    /**
     * @brief Triggered when the text to speech conversion is about to start. It
     *        provides the speech ID, generated for the text input given in the speak
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnWillSpeak(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

    /**
     * @brief Triggered when the speech start.
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnSpeechStart(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

    /**
     * @brief Triggered when the ongoing speech pauses.
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnSpeechPause(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

    /**
     * @brief Triggered when any paused speech resumes.
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnSpeechResume(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

    /**
     * @brief Triggered when the speech completes.
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnSpeechComplete(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

    /**
     * @brief Triggered when the current speech is interrupted either by a next
     *        speech request, by calling cancel or by disabling TTS, when speech is in
     *        progress.
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId>
    subscribeOnSpeechInterrupted(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

    /**
     * @brief Triggered when an error occurs during playback including network
     *        failures
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnNetworkError(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

    /**
     * @brief Triggered when an error occurs during playback including pipeline
     *        failures
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnPlaybackError(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

    /**
     * @brief Remove subscriber from subscribers list. This method is generic for
     *        all subscriptions
     *
     * @param[in] id : The subscription id
     *
     * @retval The status
     */
    virtual Result<void> unsubscribe(SubscriptionId id) = 0;

    /**
     * @brief Remove all active subscriptions from subscribers list.
     */
    virtual void unsubscribeAll() = 0;
};
} // namespace Firebolt::TextToSpeech
