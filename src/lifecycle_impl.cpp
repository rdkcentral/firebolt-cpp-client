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
    auto lastSeen = std::make_shared<std::optional<LifecycleState>>();
    std::function<void(const std::vector<StateChange>&)> wrapper =
        [this, lastSeen, notification = std::move(notification)](const std::vector<StateChange>& changes)
    {
        if (!changes.empty())
        {
            // A change ending where this subscriber already is, without starting there, repeats one it
            // has seen (e.g. a held event after a synthesized resume change); round trips still pass.
            if (*lastSeen == changes.back().newState && changes.front().oldState != changes.back().newState)
            {
                return;
            }
            *lastSeen = changes.back().newState;
            lastDelivered_ = changes.back().newState;
        }
        notification(changes);
    };
    return subscriptionManager_
        .subscribe<Firebolt::JSON::NL_Json_Array<JsonData::StateChange, StateChange>>("Lifecycle2.onStateChanged",
                                                                                      std::move(wrapper));
}

Result<void> LifecycleImpl::unsubscribe(SubscriptionId id)
{
    return subscriptionManager_.unsubscribe(id);
}

void LifecycleImpl::unsubscribeAll()
{
    subscriptionManager_.unsubscribeAll();
}
void LifecycleImpl::reconcile()
{
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
