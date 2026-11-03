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
    {
        std::lock_guard<std::recursive_mutex> lock(stateMutex_);
        if (subscribers_.empty())
        {
            lastDelivered_.reset();
        }
    }

    auto state = std::make_shared<SubscriberState>(std::move(notification));
    std::function<void(const std::vector<StateChange>&)> wrapper = [this, state](const std::vector<StateChange>& changes)
    {
        std::lock_guard<std::recursive_mutex> dispatchLock(dispatchMutex_);
        std::vector<StateChange> filtered;
        {
            std::lock_guard<std::recursive_mutex> stateLock(stateMutex_);
            if (!state->active)
            {
                return;
            }

            auto pendingCorrection = state->pendingCorrection;
            state->pendingCorrection.reset();
            for (const StateChange& change : changes)
            {
                if (pendingCorrection && pendingCorrection->first == change.oldState &&
                    pendingCorrection->second == change.newState)
                {
                    pendingCorrection.reset();
                    continue;
                }
                filtered.push_back(change);
            }
            if (!filtered.empty())
            {
                lastDelivered_ = filtered.back().newState;
            }
        }
        if (!filtered.empty())
        {
            state->notification(filtered);
        }
    };
    auto result =
        subscriptionManager_
            .subscribe<Firebolt::JSON::NL_Json_Array<JsonData::StateChange, StateChange>>("Lifecycle2.onStateChanged",
                                                                                          std::move(wrapper));
    if (result)
    {
        std::lock_guard<std::recursive_mutex> lock(stateMutex_);
        subscribers_.emplace(result.value(), std::move(state));
    }
    return result;
}

Result<void> LifecycleImpl::unsubscribe(SubscriptionId id)
{
    {
        std::lock_guard<std::recursive_mutex> lock(stateMutex_);
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
    return subscriptionManager_.unsubscribe(id);
}

void LifecycleImpl::unsubscribeAll()
{
    {
        std::lock_guard<std::recursive_mutex> lock(stateMutex_);
        for (auto& subscriber : subscribers_)
        {
            subscriber.second->active = false;
        }
        subscribers_.clear();
        lastDelivered_.reset();
    }
    subscriptionManager_.unsubscribeAll();
}
void LifecycleImpl::reconcile()
{
    std::lock_guard<std::recursive_mutex> dispatchLock(dispatchMutex_);
    std::optional<LifecycleState> previous;
    std::vector<std::pair<SubscriptionId, std::shared_ptr<SubscriberState>>> recipients;
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
        previous = lastDelivered_;
        recipients.assign(subscribers_.begin(), subscribers_.end());
    }

    Result<LifecycleState> current = state();
    if (!current)
    {
        FIREBOLT_LOG_WARNING("Lifecycle", "Cannot reconcile state after resume, error: %d",
                             static_cast<int>(current.error()));
        return;
    }
    if (*current == *previous)
    {
        return;
    }

    std::vector<std::shared_ptr<SubscriberState>> activeRecipients;
    {
        std::lock_guard<std::recursive_mutex> lock(stateMutex_);
        if (!lastDelivered_ || *lastDelivered_ != *previous)
        {
            return;
        }
        for (const auto& recipient : recipients)
        {
            auto it = subscribers_.find(recipient.first);
            if (it != subscribers_.end() && it->second == recipient.second && recipient.second->active)
            {
                recipient.second->pendingCorrection = std::make_pair(*previous, *current);
                activeRecipients.push_back(recipient.second);
            }
        }
        if (activeRecipients.empty())
        {
            return;
        }
        lastDelivered_ = *current;
    }

    const StateChange change{*previous, *current};
    FIREBOLT_LOG_NOTICE("Lifecycle", "Reconciling state after resume: %s -> %s",
                        Firebolt::JSON::toString(JsonData::LifecycleStateEnum, *previous).c_str(),
                        Firebolt::JSON::toString(JsonData::LifecycleStateEnum, *current).c_str());
    const std::vector<StateChange> changes{change};
    for (const auto& recipient : activeRecipients)
    {
        bool active = false;
        {
            std::lock_guard<std::recursive_mutex> lock(stateMutex_);
            active = recipient->active;
        }
        if (active)
        {
            recipient->notification(changes);
        }
    }
}
} // namespace Firebolt::Lifecycle
