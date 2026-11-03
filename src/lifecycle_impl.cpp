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

#include "lifecycle_impl.h"
#include "json_types/lifecycle.h"
#include <cctype>
#include <firebolt/logger.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

namespace Firebolt::Lifecycle
{
LifecycleImpl::LifecycleImpl(Firebolt::Helpers::IHelper& helper)
    : helper_(helper),
      subscriptionManager_(helper, this)
{
}

LifecycleImpl::~LifecycleImpl() = default;

Result<void> LifecycleImpl::close(const CloseType& reason) const
{
    nlohmann::json params;
    params["type"] = Firebolt::JSON::toString(JsonData::CloseReasonEnum, reason);
    return helper_.invoke("Lifecycle2.close", params);
}

Result<LifecycleState> LifecycleImpl::state() const
{
    return helper_.get<JsonData::LifecycleState, LifecycleState>("Lifecycle2.state");
}

Result<SubscriptionId>
LifecycleImpl::subscribeOnStateChanged(std::function<void(const std::vector<StateChange>&)>&& notification)
{
    std::lock_guard<std::recursive_mutex> lock(stateMutex_);
    if (subscribers_.empty())
    {
        lastDelivered_.reset();
    }

    auto state = std::make_shared<SubscriberState>();
    std::function<void(const std::vector<StateChange>&)> wrapper =
        [this, state, notification = std::move(notification)](const std::vector<StateChange>& changes)
    {
        std::vector<StateChange> filtered;
        {
            std::lock_guard<std::recursive_mutex> lock(stateMutex_);
            if (!state->active)
            {
                return;
            }

            std::optional<LifecycleState> current = state->lastSeen;
            for (const StateChange& change : changes)
            {
                if (current && *current == change.newState && change.oldState != change.newState)
                {
                    continue;
                }
                filtered.push_back(change);
                current = change.newState;
            }
            if (!filtered.empty())
            {
                state->lastSeen = current;
                lastDelivered_ = current;
            }
        }
        if (!filtered.empty())
        {
            notification(filtered);
        }
    };
    auto result =
        subscriptionManager_
            .subscribe<Firebolt::JSON::NL_Json_Array<JsonData::StateChange, StateChange>>("Lifecycle2.onStateChanged",
                                                                                          std::move(wrapper));
    if (result)
    {
        subscribers_.emplace(result.value(), std::move(state));
    }
    else if (subscribers_.empty())
    {
        lastDelivered_.reset();
    }
    return result;
}

Result<void> LifecycleImpl::unsubscribe(SubscriptionId id)
{
    std::lock_guard<std::recursive_mutex> lock(stateMutex_);
    auto result = subscriptionManager_.unsubscribe(id);
    if (result)
    {
        auto it = subscribers_.find(id);
        if (it != subscribers_.end())
        {
            it->second->active = false;
            subscribers_.erase(it);
        }
        if (subscribers_.empty())
        {
            lastDelivered_.reset();
        }
    }
    return result;
}

void LifecycleImpl::unsubscribeAll()
{
    std::lock_guard<std::recursive_mutex> lock(stateMutex_);
    subscriptionManager_.unsubscribeAll();
    for (auto& subscriber : subscribers_)
    {
        subscriber.second->active = false;
    }
    subscribers_.clear();
    lastDelivered_.reset();
}
void LifecycleImpl::reconcile()
{
    std::lock_guard<std::recursive_mutex> lock(stateMutex_);
    if (subscribers_.empty())
    {
        lastDelivered_.reset();
        return;
    }
    if (!lastDelivered_)
    {
        return;
    }
    Result<LifecycleState> current = state();
    if (!current)
    {
        FIREBOLT_LOG_WARNING("Lifecycle", "Cannot reconcile state after resume, error: %d",
                             static_cast<int>(current.error()));
        return;
    }
    if (*current == *lastDelivered_)
    {
        return;
    }
    const nlohmann::json change = {
        {"oldState", Firebolt::JSON::toString(JsonData::LifecycleStateEnum, *lastDelivered_)},
        {"newState", Firebolt::JSON::toString(JsonData::LifecycleStateEnum, *current)},
    };
    FIREBOLT_LOG_NOTICE("Lifecycle", "Reconciling state after resume: %s", change.dump().c_str());
    (void)subscriptionManager_.dispatch("Lifecycle2.onStateChanged", nlohmann::json::array({change}));
}
} // namespace Firebolt::Lifecycle
