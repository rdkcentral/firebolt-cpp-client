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
#include <iostream>
#include <string>
#include <vector>

using namespace Firebolt;
using namespace Firebolt::Lifecycle;

#include "outputstream.h"
extern OutputStream gOutput;

LifecycleDemo::LifecycleDemo()
    : FireboltDemoBase()
{
    methodsFromRpc("Lifecycle2");
    itemDescriptions_.push_back(
        {"Trigger lifecycle state change", "Simulate a lifecycle state change event from the platform."});
    currentState_ = LifecycleState::INITIALIZING;
}

void LifecycleDemo::runOption(const int index)
{
    std::string key = itemDescriptions_[index].name;

    if (key == "Lifecycle2.close")
    {
        CloseType type = chooseEnumFromList(Firebolt::Lifecycle::JsonData::CloseReasonEnum, "Choose Close Type:");
        Result<void> r = Firebolt::IFireboltAccessor::Instance().LifecycleInterface().close(type);
        validateResult(r);
    }
    else if (key == "Lifecycle2.state")
    {
        Result<LifecycleState> r = Firebolt::IFireboltAccessor::Instance().LifecycleInterface().state();
        if (validateResult(r))
        {
            currentState_ = r.value();
            printCurrentState();
        }
    }
    else if (key == "Lifecycle2.onStateChanged")
    {
        auto callback = [&](const std::vector<StateChange>& changes)
        {
            gOutput << "Lifecycle State Changes received:" << std::endl;
            for (const auto& change : changes)
            {
                gOutput << "  From "
                        << Firebolt::JSON::toString(Firebolt::Lifecycle::JsonData::LifecycleStateEnum, change.oldState)
                        << " to "
                        << Firebolt::JSON::toString(Firebolt::Lifecycle::JsonData::LifecycleStateEnum, change.newState)
                        << std::endl;
                currentState_ = change.newState;
            }
        };
        Result<SubscriptionId> r =
            Firebolt::IFireboltAccessor::Instance().LifecycleInterface().subscribeOnStateChanged(std::move(callback));
        if (validateResult(r))
        {
            gOutput << "Subscribed to Lifecycle state changes with Subscription ID: " << r.value() << std::endl;
        }
    }
    else if (key == "Trigger lifecycle state change")
    {
        triggerLifecycleStateChange();
    }
    else if (key == "Lifecycle2.unsubscribe")
    {
        std::string idStr;
        paramFromConsole("Subscription ID to unsubscribe", "0", idStr);
        SubscriptionId id = static_cast<SubscriptionId>(std::stoul(idStr));
        Result<void> r = Firebolt::IFireboltAccessor::Instance().LifecycleInterface().unsubscribe(id);
        validateResult(r);
    }
    else if (key == "Lifecycle2.unsubscribeAll")
    {
        Firebolt::IFireboltAccessor::Instance().LifecycleInterface().unsubscribeAll();
        gOutput << "Unsubscribed from all Lifecycle subscriptions." << std::endl;
    }
}

void LifecycleDemo::triggerLifecycleStateChange()
{
    LifecycleState newState = LifecycleState::ACTIVE;
    if (gAutoRun)
    {
        gOutput << "Auto-selecting 'active' for new Lifecycle State." << std::endl;
    }
    else
    {
        newState = chooseEnumFromList(Firebolt::Lifecycle::JsonData::LifecycleStateEnum,
                                      "Choose new Lifecycle State to simulate:");
    }

    nlohmann::json params;
    params["value"] = nlohmann::json::array();
    params["value"].push_back(
        {{"newState", Firebolt::JSON::toString(Firebolt::Lifecycle::JsonData::LifecycleStateEnum, newState)},
         {"oldState", Firebolt::JSON::toString(Firebolt::Lifecycle::JsonData::LifecycleStateEnum, currentState_)}});
    triggerEvent("Lifecycle2.onStateChanged", params.dump());
}

void LifecycleDemo::printCurrentState()
{
    gOutput << "Current Lifecycle State: "
            << stringFromEnum(Firebolt::Lifecycle::JsonData::LifecycleStateEnum, currentState_) << std::endl;
}