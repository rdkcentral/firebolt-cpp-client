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

#include "firebolt/lifecycle.h"
#include <firebolt/helpers.h>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <utility>

namespace Firebolt::Lifecycle
{
struct LifecycleEvent
{
    LifecycleState state;
    LifecycleState previous;
};

class LifecycleImpl : public ILifecycle
{
public:
    explicit LifecycleImpl(Firebolt::Helpers::IHelper& helper);
    LifecycleImpl(const LifecycleImpl&) = delete;
    LifecycleImpl& operator=(const LifecycleImpl&) = delete;
    LifecycleImpl(LifecycleImpl&&) = delete;
    LifecycleImpl& operator=(LifecycleImpl&&) = delete;
    ~LifecycleImpl() override;

    [[nodiscard]] Result<void> close(const CloseType& type) const override;
    [[nodiscard]] Result<LifecycleState> state() const override;

    Result<SubscriptionId>
    subscribeOnStateChanged(std::function<void(const std::vector<StateChange>&)>&& notification) override;

    Result<void> unsubscribe(SubscriptionId id) override;
    void unsubscribeAll() override;

    // Called after a transport resume: queries Lifecycle2.state and, if it differs from the last
    // state delivered to the app, delivers one corrective {last -> current} change to subscribers.
    void reconcile();

private:
    struct SubscriberState
    {
        explicit SubscriberState(std::function<void(const std::vector<StateChange>&)> callback)
            : notification(std::move(callback))
        {
        }

        std::function<void(const std::vector<StateChange>&)> notification;
        std::optional<std::pair<LifecycleState, LifecycleState>> pendingCorrection;
        bool active{true};
    };

    Firebolt::Helpers::IHelper& helper_;
    Firebolt::Helpers::SubscriptionManager subscriptionManager_;
    std::recursive_mutex dispatchMutex_;
    std::recursive_mutex stateMutex_;
    std::map<SubscriptionId, std::shared_ptr<SubscriberState>> subscribers_;
    std::optional<LifecycleState> lastDelivered_;
};
} // namespace Firebolt::Lifecycle
