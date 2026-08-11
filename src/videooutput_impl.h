/**
 * Copyright 2026 Comcast Cable Communications Management, LLC
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
#ifndef FIREBOLT_VIDEOOUTPUT_IMPL_H
#define FIREBOLT_VIDEOOUTPUT_IMPL_H

#include "firebolt/videooutput.h"
#include <firebolt/helpers.h>

namespace Firebolt::VideoOutput
{

class VideoOutputImpl : public IVideoOutput
{
public:
    explicit VideoOutputImpl(Firebolt::Helpers::IHelper& helper);
    VideoOutputImpl(const VideoOutputImpl&) = delete;
    VideoOutputImpl& operator=(const VideoOutputImpl&) = delete;
    VideoOutputImpl(VideoOutputImpl&&) = delete;
    VideoOutputImpl& operator=(VideoOutputImpl&&) = delete;
    ~VideoOutputImpl() override = default;
    [[nodiscard]] Result<VideoOutputResolution> resolution() const override;
    Result<SubscriptionId>
    subscribeOnResolutionChanged(std::function<void(const VideoOutputResolution&)>&& notification) override;

    [[nodiscard]] Result<HdcpState> hdcp() const override;
    Result<SubscriptionId> subscribeOnHdcpChanged(std::function<void(const HdcpState&)>&& notification) override;

    [[nodiscard]] Result<CecStateValue> cecState() const override;
    Result<SubscriptionId> subscribeOnCecStateChanged(std::function<void(const CecStateValue&)>&& notification) override;

    [[nodiscard]] Result<RefreshRateValue> refreshRate() const override;
    Result<SubscriptionId>
    subscribeOnRefreshRateChanged(std::function<void(const RefreshRateValue&)>&& notification) override;

    [[nodiscard]] Result<ColorDepthValue> colorDepth() const override;

    [[nodiscard]] Result<ColorFormatValue> colorFormat() const override;

    [[nodiscard]] Result<OutputColorimetry> colorimetry() const override;

    [[nodiscard]] Result<DynamicRangeValue> dynamicRange() const override;

    [[nodiscard]] Result<QuantizationRangeValue> quantizationRange() const override;

    Result<void> unsubscribe(SubscriptionId id) override;
    void unsubscribeAll() override;

private:
    Firebolt::Helpers::IHelper& helper_;
    Firebolt::Helpers::SubscriptionManager subscriptionManager_;
};

} // namespace Firebolt::VideoOutput

#endif // FIREBOLT_VIDEOOUTPUT_IMPL_H
