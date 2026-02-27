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

#include "firebolt/metrics.h"
#include <firebolt/helpers.h>

namespace Firebolt::Metrics
{
class MetricsImpl : public IMetrics
{
public:
    explicit MetricsImpl(Firebolt::Helpers::IHelper& helper);
    MetricsImpl(const MetricsImpl&) = delete;
    MetricsImpl& operator=(const MetricsImpl&) = delete;

    ~MetricsImpl() override = default;

    Result<bool> ready() const override;
    Result<bool> signIn() const override;
    Result<bool> signOut() const override;
    Result<bool> startContent(const std::optional<std::string>& entityId,
                              const std::optional<Firebolt::AgePolicy> agePolicy) const override;
    Result<bool> stopContent(const std::optional<std::string>& entityId,
                             const std::optional<Firebolt::AgePolicy> agePolicy) const override;
    Result<bool> page(const std::string& pageId, const std::optional<Firebolt::AgePolicy>& agePolicy) const override;
    Result<bool> error(const ErrorType type, const std::string& code, const std::string& description,
                       const bool visible, const std::optional<std::map<std::string, std::string>>& parameters,
                       const std::optional<Firebolt::AgePolicy>& agePolicy) const override;
    Result<bool> mediaLoadStart(const std::string& entityId,
                                const std::optional<Firebolt::AgePolicy>& agePolicy) const override;
    Result<bool> mediaPlay(const std::string& entityId,
                           const std::optional<Firebolt::AgePolicy>& agePolicy) const override;
    Result<bool> mediaPlaying(const std::string& entityId,
                              const std::optional<Firebolt::AgePolicy>& agePolicy) const override;
    Result<bool> mediaPause(const std::string& entityId,
                            const std::optional<Firebolt::AgePolicy>& agePolicy) const override;
    Result<bool> mediaWaiting(const std::string& entityId,
                              const std::optional<Firebolt::AgePolicy>& agePolicy) const override;
    Result<bool> mediaSeeking(const std::string& entityId, const double target,
                              const std::optional<Firebolt::AgePolicy>& agePolicy) const override;
    Result<bool> mediaSeeked(const std::string& entityId, const double position,
                             const std::optional<Firebolt::AgePolicy>& agePolicy) const override;
    Result<bool> mediaRateChanged(const std::string& entityId, const double rate,
                                  const std::optional<Firebolt::AgePolicy>& agePolicy) const override;
    Result<bool> mediaRenditionChanged(const std::string& entityId, const unsigned bitrate, const unsigned width,
                                       const unsigned height, const std::optional<std::string>& profile,
                                       const std::optional<Firebolt::AgePolicy>& agePolicy) const override;
    Result<bool> mediaEnded(const std::string& entityId,
                            const std::optional<Firebolt::AgePolicy>& agePolicy) const override;
    Result<bool> event(const std::string& schema, const std::string& data,
                       const std::optional<Firebolt::AgePolicy>& agePolicy) const override;
    Result<void> appInfo(const std::string& build) const override;

private:
    Firebolt::Helpers::IHelper& helper_;
};
} // namespace Firebolt::Metrics
