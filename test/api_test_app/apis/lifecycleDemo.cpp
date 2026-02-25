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

#include "lifecycleDemo.h"
#include "json_types/jsondata_lifecycle_types.h"
#include "utils.h"
#include <firebolt/firebolt.h>
#include <iostream>
#include <string>
#include <vector>

using namespace Firebolt;
using namespace Firebolt::Lifecycle;

LifecycleDemo::LifecycleDemo()
    : DemoBase("Lifecycle")
{
    methods_.push_back("Lifecycle2.close");
    methods_.push_back("Lifecycle2.state");
    methods_.push_back("Lifecycle2.onStateChanged");
    methods_.push_back("Lifecycle2.unsubscribe");
    methods_.push_back("Lifecycle2.unsubscribeAll");
}

void LifecycleDemo::runOption(const std::string& method)
{
    if (method == "Lifecycle2.close")
    {
        CloseType type = chooseEnumFromList(Firebolt::Lifecycle::JsonData::CloseReasonEnum, "Choose Close Type:");
        auto r = Firebolt::IFireboltAccessor::Instance().LifecycleInterface().close(type);
        succeed(r);
    }
    else if (method == "Lifecycle2.state")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().LifecycleInterface().state();
        if (succeed(r))
        {
            std::cout << "Current Lifecycle State: "
                      << Firebolt::JSON::toString(Firebolt::Lifecycle::JsonData::LifecycleStateEnum, *r) << std::endl;
        }
    }
    else if (method == "Lifecycle2.onStateChanged")
    {
        auto callback = [&](const std::vector<StateChange>& changes)
        {
            std::cout << "Lifecycle State Changes received:" << std::endl;
            for (const auto& change : changes)
            {
                std::cout << "  From "
                          << Firebolt::JSON::toString(Firebolt::Lifecycle::JsonData::LifecycleStateEnum, change.oldState)
                          << " to "
                          << Firebolt::JSON::toString(Firebolt::Lifecycle::JsonData::LifecycleStateEnum, change.newState)
                          << std::endl;
                currentState_ = change.newState;
            }
        };
        auto r =
            Firebolt::IFireboltAccessor::Instance().LifecycleInterface().subscribeOnStateChanged(std::move(callback));
        if (succeed(r))
        {
            std::cout << "Subscribed to Lifecycle state changes with Subscription ID: " << *r << std::endl;
        }
    }
    else if (method == "Lifecycle2.unsubscribe")
    {
        SubscriptionId id = 0;
        try
        {
            id = static_cast<SubscriptionId>(std::stoul(paramFromConsole("Subscription ID to unsubscribe", "0")));
        }
        catch (const std::exception&)
        {
        }
        auto r = Firebolt::IFireboltAccessor::Instance().LifecycleInterface().unsubscribe(id);
        succeed(r);
    }
    else if (method == "Lifecycle2.unsubscribeAll")
    {
        Firebolt::IFireboltAccessor::Instance().LifecycleInterface().unsubscribeAll();
        std::cout << "Unsubscribed from all Lifecycle subscriptions." << std::endl;
    }
}
