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

#include "device_impl.h"
#include "json_types/device.h"

namespace Firebolt::Device
{
DeviceImpl::DeviceImpl(Firebolt::Helpers::IHelper& helper)
    : helper_(helper),
      subscriptionManager_(helper, this)
{
}

Result<std::string> DeviceImpl::uid() const
{
    return helper_.get<Firebolt::JSON::String, std::string>("Device.uid");
}

Result<HDRFormatMap> DeviceImpl::hdr() const
{
    return Result(helper_.get<JsonData::HDRFormatMap, HDRFormatMap>("Device.hdr"));
}

Result<SubscriptionId> DeviceImpl::subscribeOnHdr(std::function<void(const HDRFormatMap&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::HDRFormatMap>("Device.onHdr", std::move(notification));
}

Result<void> DeviceImpl::unsubscribe(SubscriptionId id)
{
    return subscriptionManager_.unsubscribe(id);
}

void DeviceImpl::unsubscribeAll()
{
    subscriptionManager_.unsubscribeAll();
}
} // namespace Firebolt::Device
