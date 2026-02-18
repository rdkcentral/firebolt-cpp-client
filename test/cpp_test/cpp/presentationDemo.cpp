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
#include <iostream>
#include <string>
#include <vector>

#include "utils.h"

#include "outputstream.h"
extern OutputStream gOutput;

using namespace Firebolt;
using namespace Firebolt::Presentation;

PresentationDemo::PresentationDemo()
    : FireboltDemoBase()
{
    methodsFromRpc("Presentation");
    itemDescriptions_.push_back(
        {"Trigger presentation state change", "Simulate a presentation state change event from the platform."});
}

void PresentationDemo::runOption(const int index)
{
    std::string key = itemDescriptions_[index].name;

    if (key == "Presentation.focused")
    {
        Result<bool> r = Firebolt::IFireboltAccessor::Instance().PresentationInterface().focused();
        if (validateResult(r))
        {
            gOutput << "Presentation focused: " << (r.value() ? "true" : "false") << std::endl;
        }
    }
    else if (key == "Presentation.onFocusedChanged")
    {
        auto subscriptionResult =
            Firebolt::IFireboltAccessor::Instance().PresentationInterface().subscribeOnFocusedChanged(
                [&](bool focused)
                { gOutput << "Presentation focus changed: " << (focused ? "true" : "false") << std::endl; });
        if (validateResult(subscriptionResult))
        {
            SubscriptionId subId = subscriptionResult.value();
            gOutput << "Subscribed to Presentation.onFocusedChanged with SubscriptionId: " << subId << std::endl;
        }
    }
}
