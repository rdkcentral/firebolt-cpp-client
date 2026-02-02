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

#include <firebolt/types.h>

namespace Firebolt::Discovery
{
enum class AgePolicy
{
    ADULT,
    CHILD,
    TEEN,
};

class IDiscovery
{
public:
    virtual ~IDiscovery() = default;

    /**
     * @brief Notify the platform that content was partially or completely watched
     *
     * @param[in]  entityId    : The entity Id of the watched content
     * @param[in]  progress    : How much of the content has been watched (percentage as (0-0.999) for VOD, number of
     *                           seconds for live)
     * @param[in]  completed   : Whether or not this viewing is considered "complete" per the app's definition thereof
     * @param[in]  watchedOn   : The ISO 8601 timestamp of when the content was watched
     * @param[in]  agePolicy   : The age policy associated with the watch event. The age policy describes the age groups
     *                           to which content may be directed
     *
     * @retval The status.
     */
    virtual Result<bool> watched(const std::string& entityId, std::optional<float> progress,
                                 std::optional<bool> completed, std::optional<std::string> watchedOn,
                                 std::optional<AgePolicy> agePolicy) const = 0;
};
} // namespace Firebolt::Discovery
