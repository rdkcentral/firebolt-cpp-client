/**
 * Copyright 2025 Comcast Cable Communications Management, LLC
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

#include "mock_helper.h"
#include <map>
#include <memory>
#include <vector>

// Behavioural fake for IHelper subscription handling: stores subscriptions like HelperImpl,
// delivers wire events via emit() and local events via dispatchLocal().
class FakeSubscriptions
{
public:
    explicit FakeSubscriptions(MockHelper& helper)
    {
        using ::testing::_;
        using ::testing::Invoke;

        ON_CALL(helper, subscribe(_, _, _, _))
            .WillByDefault(Invoke(
                [this](void* owner, const std::string& eventName, std::any&& notification,
                       void (*callback)(void*, const nlohmann::json&))
                {
                    const Firebolt::SubscriptionId id = nextId_++;
                    auto data = std::make_shared<Firebolt::Helpers::SubscriptionData>(
                        Firebolt::Helpers::SubscriptionData{owner, eventName, std::move(notification)});
                    entries_[id] = Entry{std::move(data), callback};
                    return Firebolt::Result<Firebolt::SubscriptionId>{id};
                }));
        ON_CALL(helper, unsubscribe(_))
            .WillByDefault(Invoke(
                [this](Firebolt::SubscriptionId id)
                {
                    entries_.erase(id);
                    return Firebolt::Result<void>{Firebolt::Error::None};
                }));
        ON_CALL(helper, unsubscribeAll(_))
            .WillByDefault(Invoke(
                [this](void* owner)
                {
                    for (auto it = entries_.begin(); it != entries_.end();)
                    {
                        it = (it->second.data->owner == owner) ? entries_.erase(it) : std::next(it);
                    }
                }));
        ON_CALL(helper, dispatchLocal(_, _, _))
            .WillByDefault(Invoke(
                [this](void* owner, const std::string& eventName, const nlohmann::json& payload)
                {
                    deliver(&owner, eventName, payload);
                    return Firebolt::Result<void>{Firebolt::Error::None};
                }));
    }

    // Simulates an event arriving from the gateway for every subscriber of eventName.
    void emit(const std::string& eventName, const nlohmann::json& payload) { deliver(nullptr, eventName, payload); }

    size_t count() const { return entries_.size(); }

private:
    struct Entry
    {
        std::shared_ptr<Firebolt::Helpers::SubscriptionData> data;
        void (*callback)(void*, const nlohmann::json&);
    };

    void deliver(void* const* owner, const std::string& eventName, const nlohmann::json& payload)
    {
        std::vector<Entry> targets;
        for (const auto& [id, entry] : entries_)
        {
            if (entry.data->eventName == eventName && (owner == nullptr || entry.data->owner == *owner))
            {
                targets.push_back(entry);
            }
        }
        for (const auto& entry : targets)
        {
            entry.callback(entry.data.get(), payload);
        }
    }

    std::map<Firebolt::SubscriptionId, Entry> entries_;
    Firebolt::SubscriptionId nextId_{1};
};
