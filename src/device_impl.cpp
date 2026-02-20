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
#include "jsondata_device_types.h"

namespace Firebolt::Device
{
DeviceImpl::DeviceImpl(Firebolt::Helpers::IHelper& helper)
    : helper_(helper),
      subscriptionManager_(helper, this)
{
}

Result<std::string> DeviceImpl::chipsetId() const
{
    return helper_.get<Firebolt::JSON::String, std::string>("Device.chipsetId");
}

Result<DeviceClass> DeviceImpl::deviceClass() const
{
    return Result(helper_.get<JsonData::DeviceClassJson, DeviceClass>("Device.deviceClass"));
}

Result<HDRFormat> DeviceImpl::hdr() const
{
    return Result(helper_.get<JsonData::HDRFormat, HDRFormat>("Device.hdr"));
}

Result<u_int32_t> DeviceImpl::timeInActiveState() const
{
    return helper_.get<Firebolt::JSON::Unsigned, u_int32_t>("Device.timeInActiveState");
}

Result<std::string> DeviceImpl::uid() const
{
    return helper_.get<Firebolt::JSON::String, std::string>("Device.uid");
}

Result<uint32_t> DeviceImpl::uptime() const
{
    return helper_.get<Firebolt::JSON::Unsigned, uint32_t>("Device.uptime");
}

Result<SubscriptionId> DeviceImpl::subscribeOnHdrChanged(std::function<void(const HDRFormat&)>&& notification)
{
    return subscriptionManager_.subscribe<JsonData::HDRFormat>("Device.onHdrChanged", std::move(notification));
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
