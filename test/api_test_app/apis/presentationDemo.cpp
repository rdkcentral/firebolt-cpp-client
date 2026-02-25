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

#include "presentationDemo.h"
#include <firebolt/firebolt.h>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

using namespace Firebolt;

PresentationDemo::PresentationDemo()
    : DemoBase("Presentation")
{
    methods_.push_back("Presentation.focused");
    methods_.push_back("Presentation.onFocusedChanged");
}

void PresentationDemo::runOption(const std::string& method)
{
    if (method == "Presentation.focused")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().PresentationInterface().focused();
        if (succeed(r))
        {
            std::cout << "Presentation focused: " << std::boolalpha << *r << std::endl;
        }
    }
    else if (method == "Presentation.onFocusedChanged")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().PresentationInterface().subscribeOnFocusedChanged(
            [&](bool focused) { std::cout << "Presentation focus changed: " << std::boolalpha << focused << std::endl; });
        if (succeed(r))
        {
            std::cout << "Subscribed to Presentation.onFocusedChanged with SubscriptionId: " << *r << std::endl;
        }
    }
}
