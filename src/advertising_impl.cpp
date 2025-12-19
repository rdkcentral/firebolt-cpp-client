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

#include "advertising_impl.h"
#include "jsondata_advertising_types.h"

namespace Firebolt::Advertising
{
AdvertisingImpl::AdvertisingImpl(Firebolt::Helpers::IHelper& helper)
    : helper_(helper)
{
}

Result<Ifa> AdvertisingImpl::advertisingId() const
{
    return helper_.get<JsonData::IfaJson, Ifa>("Advertising.advertisingId");
}

} // namespace Firebolt::Advertising
