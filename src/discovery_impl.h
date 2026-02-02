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

#include "firebolt/discovery.h"
#include <firebolt/helpers.h>

namespace Firebolt::Discovery
{
class DiscoveryImpl : public IDiscovery
{
public:
    explicit DiscoveryImpl(Firebolt::Helpers::IHelper& helper);
    DiscoveryImpl(const DiscoveryImpl&) = delete;
    DiscoveryImpl& operator=(const DiscoveryImpl&) = delete;

    ~DiscoveryImpl() override = default;

    Result<bool> watched(const std::string& entityId, std::optional<float> progress, std::optional<bool> completed,
                         std::optional<std::string> watchedOn, std::optional<AgePolicy> agePolicy) const override;

private:
    Firebolt::Helpers::IHelper& helper_;
};
} // namespace Firebolt::Discovery
