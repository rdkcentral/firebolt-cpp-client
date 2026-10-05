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

#ifndef FIREBOLT_ACTIONS_JSON_H
#define FIREBOLT_ACTIONS_JSON_H

#pragma once
#include "firebolt/actions.h"
#include <firebolt/json_types.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace Firebolt::Actions::JsonData
{

// Deserialises the intent envelope while preserving the intent payload as generic JSON.
class JsonValue : public Firebolt::JSON::NL_Json_Basic<Intent>
{
public:
    void fromJson(const nlohmann::json& json) override
    {
        if (!checkRequiredFields(json, {"intent", "intentId"}))
            throw std::invalid_argument("Missing required fields in JSON");
        value_.intent = json["intent"];
        value_.intentId = json["intentId"].get<uint32_t>();
    }
    [[nodiscard]] Intent value() const override { return value_; }

private:
    Intent value_;
};

} // namespace Firebolt::Actions::JsonData

#endif // FIREBOLT_ACTIONS_JSON_H
