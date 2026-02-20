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

#include <firebolt/types.h>

#include <functional>
#include <optional>
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

// Types
using TTSStatus = int32_t;
using SpeechId = int32_t;
using SpeechState = std::string;

struct FallbackText
{
    std::optional<std::string> scenario;
    std::optional<std::string> value;
};

struct TTSEnabled
{
    TTSStatus TTS_status;
    bool isenabled;
};

struct TTSConfiguration
{
    bool success;
    std::optional<std::string> ttsendpoint;
    std::optional<std::string> ttsendpointsecured;
    std::optional<std::string> language;
    std::optional<std::string> voice;
    std::optional<int32_t> volume;
    std::optional<int32_t> primvolduckpercent;
    std::optional<int32_t> rate;
    std::optional<SpeechRate> speechrate;
    std::optional<FallbackText> fallbacktext;
};

struct TTSStatusResponse
{
    TTSStatus TTS_status;
    bool success;
};

struct SpeechIdEvent
{
    SpeechId speechid;
    std::optional<std::string> text;
};

struct TTSState
{
    bool state;
};

struct TTSVoice
{
    std::string voice;
};

struct ListVoicesResponse
{
    TTSStatus TTS_status;
    std::vector<std::string> voices;
};

struct SpeechResponse
{
    SpeechId speechid;
    TTSStatus TTS_status;
    bool success;
};

struct SpeechStateResponse
{
    SpeechState speechstate;
    TTSStatus TTS_status;
    bool success;
};

class ITextToSpeech
{
public:
    virtual ~ITextToSpeech() = default;

    // Methods
    /**
     * @brief Returns whether the TTS engine is enabled or disabled
     *
     * @retval Returns whether the TTS engine is enabled or disabled
     */
    virtual Result<TTSEnabled> isttsenabled() = 0;

    /**
     * @brief Get the list of Text to speech voices supported by the platform
     *
     * @param[in] language : Request language in ISO 3166-1 Alpha-2
     *
     * @retval The list of voices supported for the language
     */
    virtual Result<ListVoicesResponse> listvoices(const std::string& language) = 0;

    /**
     * @brief Get the TTS configuration for the platform
     *
     * @retval Settings the list of configurations
     */
    virtual Result<TTSConfiguration> getttsconfiguration() = 0;

    /**
     * @brief Speak the uttered text using the TTS engine
     *
     * @param[in] text     : String to be converted to Audio for speech
     * @param[in] callsign : App calling text to speech
     *
     * @retval Result for Speak
     */
    virtual Result<SpeechResponse> speak(const std::string& text, const std::optional<std::string>& callsign) = 0;

    /**
     * @brief Set the TTS configuration for the platform
     *
     * @param[in] ttsEndpoint        : URL for Text to Speech API
     * @param[in] ttsEndpointSecured : URL for Secured Text to Speech API
     * @param[in] language           : Language used by Text to speech
     * @param[in] voice              : Voice used by Text to speech
     * @param[in] volume             : Volume for Text to speech
     * @param[in] primVolDuckPercent : Prime Volume duck percent for Text to speech
     * @param[in] rate               : Speech rate for Text to speech
     * @param[in] speechRate         : Rate for speech
     * @param[in] fallbackText       : Fallback text for TTS
     *
     * @retval Settings the list of configurations
     */
    virtual Result<TTSStatusResponse>
    setttsconfiguration(const std::optional<std::string>& ttsEndpoint,
                        const std::optional<std::string>& ttsEndpointSecured, const std::optional<std::string>& language,
                        const std::optional<std::string>& voice, const std::optional<int32_t>& volume,
                        const std::optional<int32_t>& primVolDuckPercent, const std::optional<int32_t>& rate,
                        const std::optional<SpeechRate>& speechRate, const std::optional<FallbackText>& fallbackText) = 0;

    /**
     * @brief Pauses the speech for given speech id
     *
     * @param[in] speechId : Identifier for the speech call
     *
     * @retval Result for Pause
     */
    virtual Result<TTSStatusResponse> pause(SpeechId speechId) = 0;

    /**
     * @brief Resumes the speech for given speech id
     *
     * @param[in] speechId : Identifier for the speech call
     *
     * @retval Result for Resume
     */
    virtual Result<TTSStatusResponse> resume(SpeechId speechId) = 0;

    /**
     * @brief Cancels the speech for given speech id
     *
     * @param[in] speechId : Identifier for the speech call
     *
     * @retval Result for cancel
     */
    virtual Result<TTSStatusResponse> cancel(SpeechId speechId) = 0;

    /**
     * @brief Returns the current state of the speech request.
     *
     * @param[in] speechId : Identifier for the speech call
     *
     * @retval Result for speech state
     */
    virtual Result<SpeechStateResponse> getspeechstate(SpeechId speechId) = 0;

    // Events
    /**
     * @brief Triggered when the text to speech conversion is about to start. It
     *        provides the speech ID, generated for the text input given in the speak
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnWillspeak(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

    /**
     * @brief Triggered when the speech start.
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnSpeechstart(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

    /**
     * @brief Triggered when the ongoing speech pauses.
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnSpeechpause(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

    /**
     * @brief Triggered when any paused speech resumes.
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnSpeechresume(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

    /**
     * @brief Triggered when the speech completes.
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnSpeechcomplete(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

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
    subscribeOnSpeechinterrupted(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

    /**
     * @brief Triggered when an error occurs during playback including network
     *        failures
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnNetworkerror(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

    /**
     * @brief Triggered when an error occurs during playback including pipeline
     *        failures
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnPlaybackerror(std::function<void(const SpeechIdEvent&)>&& notification) = 0;

    /**
     * @brief Triggered when TTS is enabled or disabled
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnTTSstatechanged(std::function<void(const TTSState&)>&& notification) = 0;

    /**
     * @brief Triggered when the configured voice changes.
     *
     * @param[in]  notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnVoicechanged(std::function<void(const TTSVoice&)>&& notification) = 0;

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
