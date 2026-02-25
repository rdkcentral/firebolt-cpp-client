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

#include "advertisingDemo.h"
#include <firebolt/firebolt.h>
#include <iostream>
#include <string>
#include <vector>

using namespace Firebolt;
using namespace Firebolt::Advertising;

AdvertisingDemo::AdvertisingDemo()
    : DemoBase("Advertising")
{
    methods_.push_back("Advertising.advertisingId");
}

void AdvertisingDemo::runOption(const std::string& method)
{
    if (method == "Advertising.advertisingId")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().AdvertisingInterface().advertisingId();
        if (succeed(r))
        {
            std::cout << "IFA: " << r->ifa << std::endl;
            std::cout << "IFA Type: " << r->ifa_type << std::endl;
            std::cout << "LMT: " << r->lmt << std::endl;
        }
    }
}
