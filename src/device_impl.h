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

#include "firebolt/device.h"
#include <firebolt/helpers.h>

namespace Firebolt::Device
{
class DeviceImpl : public IDevice
{
public:
    explicit DeviceImpl(Firebolt::Helpers::IHelper& helper);
    DeviceImpl(const DeviceImpl&) = delete;
    DeviceImpl& operator=(const DeviceImpl&) = delete;

    ~DeviceImpl() override = default;

    Result<std::string> chipsetId() const override;
    Result<DeviceClass> deviceClass() const override;
    Result<HDRFormat> hdr() const override;
    Result<u_int32_t> timeInActiveState() const override;
    Result<std::string> uid() const override;
    Result<uint32_t> uptime() const override;

    Result<SubscriptionId> subscribeOnHdrChanged(std::function<void(const HDRFormat&)>&& notification) override;

    Result<void> unsubscribe(SubscriptionId id) override;
    void unsubscribeAll() override;

private:
    Firebolt::Helpers::IHelper& helper_;
    Firebolt::Helpers::SubscriptionManager subscriptionManager_;
};
} // namespace Firebolt::Device
