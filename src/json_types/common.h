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

#include "firebolt/common_types.h"
#include <cstdint>
#include <firebolt/json_types.h>
#include <limits>
#include <stdexcept>

namespace Firebolt::JsonData
{
inline const Firebolt::JSON::EnumType<::Firebolt::AgePolicy> AgePolicyEnum({
    {"app:adult", ::Firebolt::AgePolicy::ADULT},
    {"app:child", ::Firebolt::AgePolicy::CHILD},
    {"app:teen", ::Firebolt::AgePolicy::TEEN},
});

// nlohmann's get<uint32_t>() would wrap a negative number, truncate a fraction, wrap a value above 4294967295 and
// accept a boolean, so every unsigned field is parsed with these instead.
inline uint64_t parseUnsigned64(const nlohmann::json& json)
{
    if (json.is_number_unsigned() || (json.is_number_integer() && json.get<int64_t>() >= 0))
    {
        return json.get<uint64_t>();
    }
    throw std::invalid_argument("Expected an unsigned integer");
}

// Firebolt 9 `unsigned` is a 32-bit unsigned integer
inline uint32_t parseUnsigned(const nlohmann::json& json)
{
    const auto value = parseUnsigned64(json);
    if (value > std::numeric_limits<uint32_t>::max())
    {
        throw std::invalid_argument("Expected a 32-bit unsigned integer");
    }
    return static_cast<uint32_t>(value);
}

class StrictUnsigned : public Firebolt::JSON::NL_Json_Basic<uint32_t>
{
public:
    void fromJson(const nlohmann::json& json) override { value_ = parseUnsigned(json); }
    [[nodiscard]] uint32_t value() const override { return value_; }

private:
    uint32_t value_{0};
};
} // namespace Firebolt::JsonData
