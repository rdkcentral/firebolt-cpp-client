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

#include "firebolt/network.h"
#include <firebolt/helpers.h>

namespace Firebolt::Network
{
class NetworkImpl : public INetwork
{
public:
    explicit NetworkImpl(Firebolt::Helpers::IHelper& helper);
    NetworkImpl(const NetworkImpl&) = delete;
    NetworkImpl& operator=(const NetworkImpl&) = delete;

    ~NetworkImpl() override = default;

    Result<bool> connected() const override;

    Result<SubscriptionId> subscribeOnConnectedChanged(std::function<void(bool)>&& notification) override;

    Result<void> unsubscribe(SubscriptionId id) override;
    void unsubscribeAll() override;

private:
    Firebolt::Helpers::IHelper& helper_;
    Firebolt::Helpers::SubscriptionManager subscriptionManager_;
};
} // namespace Firebolt::Network
