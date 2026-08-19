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

#include <firebolt/helpers.h>
#include <firebolt/speechsynthesis.h>

namespace Firebolt::SpeechSynthesis
{
class SpeechSynthesisImpl : public ISpeechSynthesis
{
public:
    explicit SpeechSynthesisImpl(Firebolt::Helpers::IHelper& helper);
    SpeechSynthesisImpl(const SpeechSynthesisImpl&) = delete;
    SpeechSynthesisImpl& operator=(const SpeechSynthesisImpl&) = delete;

    ~SpeechSynthesisImpl() override = default;

    [[nodiscard]] Result<unsigned> speak(const std::string& text, std::optional<std::string> callSign = std::nullopt,
                                         std::optional<std::string> language = std::nullopt,
                                         std::optional<std::string> voice = std::nullopt,
                                         std::optional<std::string> volume = std::nullopt,
                                         std::optional<std::string> rate = std::nullopt,
                                         std::optional<std::string> pitch = std::nullopt) const override;
    [[nodiscard]] Result<std::pmr::vector<Voice>> voices() const override;

    [[nodiscard]] Result<SubscriptionId>
    subscribeOnVoicesChanged(std::function<void(const std::pmr::vector<Voice>&)>&& notification) override;

    [[nodiscard]] Result<void> cancel(UtteranceId id) const override;
    [[nodiscard]] Result<void> pause(UtteranceId id) const override;
    [[nodiscard]] Result<void> resume(UtteranceId id) const override;
    [[nodiscard]] Result<SubscriptionId>
    subscribeOnUtteranceEvent(std::function<void(const UtteranceEvent&)>&& notification) override;
     Result<void> unsubscribe(SubscriptionId id) override;
    void unsubscribeAll() override;

private:
    Firebolt::Helpers::IHelper& helper_;
    Firebolt::Helpers::SubscriptionManager subscriptionManager_;
};
} // namespace Firebolt::SpeechSynthesis