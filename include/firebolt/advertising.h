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
#include <string>

namespace Firebolt::Advertising
{
// Types
struct Ifa
{
    /* UUID*/
    std::string ifa;

    /* Advertising ID type
        One of
        "dpid" - device provided ID
        "sspid" - SSP provided ID
        "sessionid" - session/synthetic ID
    */
    std::string ifa_type;

    /* Limit Ad Tracking
        One of
        "0" - LMT disabled
        "1" - LMT enabled
    */
    std::string lmt;
};

class IAdvertising
{
public:
    virtual ~IAdvertising() = default;

    /**
     * @brief Returns the IFA
     *
     * @return Ifa struct, or InvalidParams if `ifa_type` is not "dpid", "sspid" or "sessionid", or `lmt` is not "0" or "1"
     *
     */
    [[nodiscard]] virtual Result<Ifa> advertisingId() const = 0;
};
} // namespace Firebolt::Advertising
