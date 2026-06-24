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

#include "firebolt/stats.h"
#include <firebolt/json_types.h>
#include <nlohmann/json.hpp>

namespace Firebolt::Stats::JsonData
{
class MemoryInfo : public Firebolt::JSON::NL_Json_Basic<::Firebolt::Stats::MemoryInfo>
{
public:
    void fromJson(const nlohmann::json& json) override
    {
        if (!checkRequiredFields(json, {"userMemoryUsed", "userMemoryLimit", "gpuMemoryUsed", "gpuMemoryLimit"}))
        {
            throw std::invalid_argument("Missing required fields in JSON");
        }
        userMemoryUsed = json["userMemoryUsed"].get<uint64_t>();
        userMemoryLimit = json["userMemoryLimit"].get<uint64_t>();
        gpuMemoryUsed = json["gpuMemoryUsed"].get<uint64_t>();
        gpuMemoryLimit = json["gpuMemoryLimit"].get<uint64_t>();
    }
    ::Firebolt::Stats::MemoryInfo value() const override
    {
        return ::Firebolt::Stats::MemoryInfo{userMemoryUsed, userMemoryLimit, gpuMemoryUsed, gpuMemoryLimit};
    }

private:
    uint64_t userMemoryUsed;
    uint64_t userMemoryLimit;
    uint64_t gpuMemoryUsed;
    uint64_t gpuMemoryLimit;
};
} // namespace Firebolt::Stats::JsonData
