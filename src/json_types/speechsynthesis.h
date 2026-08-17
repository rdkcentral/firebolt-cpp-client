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

#include "firebolt/speechsynthesis.h"
#include <firebolt/json_types.h>

namespace Firebolt::SpeechSynthesis::JsonData
{
class VoicesResponse : public Firebolt::JSON::NL_Json_Basic<::std::pmr::vector<::Firebolt::SpeechSynthesis::Voice>>
{
public:
    void fromJson(const nlohmann::json& json) override
    {
        if (!json.is_array())
        {
            throw std::invalid_argument("Expected an array for voices response");
        }

        voices_.clear();
        for (const auto& item : json)
        {
            if (!checkRequiredFields(item, {"name", "lang", "default"}))
            {
                throw std::invalid_argument("Missing required fields in JSON");
            }

            voices_.push_back(::Firebolt::SpeechSynthesis::Voice{item["name"].get<std::string>(),
                                                                 item["lang"].get<std::string>(),
                                                                 item["default"].get<bool>()});
        }
    }

    [[nodiscard]] ::std::pmr::vector<::Firebolt::SpeechSynthesis::Voice> value() const override { return voices_; }

private:
    ::std::pmr::vector<::Firebolt::SpeechSynthesis::Voice> voices_;
};
} // namespace Firebolt::SpeechSynthesis::JsonData