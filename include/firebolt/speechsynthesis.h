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
#include <memory_resource>
#include <string>
#include <vector>
namespace Firebolt::SpeechSynthesis
{
struct Voice
{
    std::string name;
    std::string lang;
    bool _default;
};

enum class UtteranceEventEnum
{
    synthesisStarting,
    playbackStarting,
    paused,
    resumed,
    completed,
    interrupted,
    networkFailed,
    synthesisFailed,
    playbackFailed
};

using UtteranceId = std::uint32_t;

struct UtteranceEvent
{
    UtteranceId utteranceId;
    UtteranceEventEnum event;
};

class ISpeechSynthesis
{
public:
    virtual ~ISpeechSynthesis() = default;

    /**
     * @brief Requests the system to speak an utterance. A successful new request is processed asynchronously: any
     *        utterance that is currently being spoken is interrupted (UtteranceEventEnum::interrupted), and events for
     *        the new request are raised through the onUtteranceEvent subscription.
     *
     * For Voice Guidance, the app SHOULD either supply text as plain text together with lang, or supply text as a
     * well-formed SSML document. If text is plain text and lang is not supplied, the default speech synthesis language
     * is used; this is not recommended.
     *
     * @param[in] text   : Plain text or a well-formed SSML document
     * @param[in] lang   : The language of the text in BCP 47
     * @param[in] voice  : A voice name as returned by voices()
     * @param[in] volume : The speaking volume, 0 (lowest) to 1 (highest) inclusive
     * @param[in] rate   : The speaking rate, 0.1 (slowest) to 10 (fastest) inclusive
     * @param[in] pitch  : The speaking pitch, 0 (lowest) to 2 (highest) inclusive
     * @param[in] pii    : True if the text contains personally identifiable information, indicating that functionality
     *                     such as logging and telemetry should be restricted; false if not set
     *
     * @retval The utteranceId if the request is accepted, otherwise an error. The platform may answer with the specific
     *         errors -50110 (app state invalid, requires focus) or -50130 (speech synthesis engine error); the client
     *         passes platform errors through unchanged.
     */
    [[nodiscard]] virtual Result<unsigned> speak(const std::string& text, std::optional<std::string> lang = std::nullopt,
                                                 std::optional<std::string> voice = std::nullopt,
                                                 std::optional<double> volume = std::nullopt,
                                                 std::optional<double> rate = std::nullopt,
                                                 std::optional<double> pitch = std::nullopt,
                                                 std::optional<bool> pii = std::nullopt) const = 0;

    /**
     * @brief Returns a list of available voices, unordered. A voice or a language can be repeated in the list, and at
     *        most one voice is the default for each language.
     *
     * @retval The voices (name, lang in BCP 47, default for the language). The list is empty if a speech synthesis
     *         engine is not available, or the engine rejected the request; that is not an error.
     */
    [[nodiscard]] virtual Result<std::pmr::vector<Voice>> voices() const = 0;

    /**
     * @brief Subscribe to changes of the list of available voices
     *
     * @param[in] notification : The callback function, which receives the new list of voices
     *
     * @retval The subscriptionId or error
     */
    [[nodiscard]] virtual Result<SubscriptionId>
    subscribeOnVoicesChanged(std::function<void(const std::pmr::vector<Voice>&)>&& notification) = 0;

    /**
     * @brief Stops the utterance. If playing, audio is stopped. If synthesizing or paused, the utterance will not be
     *        played. Raises UtteranceEventEnum::interrupted if the utterance was synthesizing, playing or paused.
     *        Returns without error in all utterance states, or if the utterance has finished.
     *
     * @param[in] utteranceId : The utterance returned by speak()
     *
     * @retval An ok Result, or an error such as -50130 (speech synthesis engine error) passed through from the platform
     */
    [[nodiscard]] virtual Result<void> cancel(UtteranceId utteranceId) const = 0;

    /**
     * @brief Pauses the utterance. If playing, audio is stopped. If synthesizing, playback will be prevented. Raises
     *        UtteranceEventEnum::paused if the utterance was synthesizing or playing. Returns without error in all
     *        utterance states, or if the utterance has finished.
     *
     * @param[in] utteranceId : The utterance returned by speak()
     *
     * @retval An ok Result, or an error such as -50130 (speech synthesis engine error) passed through from the platform
     */
    [[nodiscard]] virtual Result<void> pause(UtteranceId utteranceId) const = 0;

    /**
     * @brief Resumes an utterance. If paused, audio is resumed. If synthesizing, the utterance will be played. If
     *        playing, no action is taken. Raises UtteranceEventEnum::resumed if the utterance had previously been
     *        paused while playing. Returns without error in all utterance states, or if the utterance has finished.
     *
     * @param[in] utteranceId : The utterance returned by speak()
     *
     * @retval An ok Result, or an error such as -50110 (app state invalid, requires focus) or -50130 (speech synthesis
     *         engine error) passed through from the platform
     */
    [[nodiscard]] virtual Result<void> resume(UtteranceId utteranceId) const = 0;

    /**
     * @brief Subscribe to events related to an utterance: progress events (synthesisStarting, playbackStarting,
     *        paused, resumed, completed, interrupted) and failure events (networkFailed, synthesisFailed,
     *        playbackFailed)
     *
     * @param[in] notification : The callback function
     *
     * @retval The subscriptionId or error
     */
    [[nodiscard]] virtual Result<SubscriptionId>
    subscribeOnUtteranceEvent(std::function<void(const UtteranceEvent&)>&& notification) = 0;

    /**
     * @brief Remove subscriber from subscribers list. This method is generic for all subscriptions
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
} // namespace Firebolt::SpeechSynthesis