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

#include <firebolt/types.h>
#include <functional>

namespace Firebolt::Network
{

class INetwork
{
public:
    virtual ~INetwork() = default;

    /**
     * @brief Returns whether the device currently has a usable network connection
     *
     * @retval The connection state or error
     */
    virtual Result<bool> connected() const = 0;

    /**
     * @brief Subscribe to connection changes
     *
     * @param notification : The callback function, which will be called on connection state changes
     *
     * @retval SubscriptionId or error
     */
    virtual Result<SubscriptionId> subscribeOnConnectedChanged(std::function<void(bool)>&& notification) = 0;

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

} // namespace Firebolt::Network
