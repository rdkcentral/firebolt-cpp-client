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

    [[nodiscard]] virtual Result<unsigned> speak(const std::string& text, std::optional<std::string> lang = std::nullopt,
                                                 std::optional<std::string> voice = std::nullopt,
                                                 std::optional<double> volume = std::nullopt,
                                                 std::optional<double> rate = std::nullopt,
                                                 std::optional<double> pitch = std::nullopt,
                                                 std::optional<bool> pii = std::nullopt) const = 0;
    [[nodiscard]] virtual Result<std::pmr::vector<Voice>> voices() const = 0;
    [[nodiscard]] virtual Result<SubscriptionId>
    subscribeOnVoicesChanged(std::function<void(const std::pmr::vector<Voice>&)>&& notification) = 0;

    [[nodiscard]] virtual Result<void> cancel(UtteranceId utteranceId) const = 0;
    [[nodiscard]] virtual Result<void> pause(UtteranceId utteranceId) const = 0;
    [[nodiscard]] virtual Result<void> resume(UtteranceId utteranceId) const = 0;
    [[nodiscard]] virtual Result<SubscriptionId>
    subscribeOnUtteranceEvent(std::function<void(const UtteranceEvent&)>&& notification) = 0;
    virtual Result<void> unsubscribe(SubscriptionId id) = 0;
    virtual void unsubscribeAll() = 0;
};
} // namespace Firebolt::SpeechSynthesis