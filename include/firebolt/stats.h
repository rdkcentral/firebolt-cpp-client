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

namespace Firebolt::Stats
{
struct MemoryInfo
{
    uint32_t userMemoryUsed;
    uint32_t userMemoryLimit;
    uint32_t gpuMemoryUsed;
    uint32_t gpuMemoryLimit;
};

class IStats
{
public:
    virtual ~IStats() = default;

    /**
     @brief Returns information about container memory usage, in units of 1024 bytes
    *
    * @retval MemoryInfo struct or error
    */
    virtual Result<MemoryInfo> memoryUsage() const = 0;
};

} // namespace Firebolt::Stats
