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

#include "actionsDemo.h"
#include <firebolt/firebolt.h>
#include <iostream>
#include <string>
#include <utility>

using namespace Firebolt;
using namespace Firebolt::Actions;

ActionsDemo::ActionsDemo()
    : DemoBase("Actions")
{
    methods_.push_back("Actions.intent");
    methods_.push_back("Actions.start");
    methods_.push_back("Actions.onIntent");
    methods_.push_back("Actions.unsubscribe");
    methods_.push_back("Actions.unsubscribeAll");
}

void ActionsDemo::runOption(const std::string& method)
{
    std::cout << "Running Actions method: " << method << std::endl;

    if (method == "Actions.intent")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().ActionsInterface().intent();
        if (succeed(r))
        {
            std::cout << "Current Intent - action: " << r->intent.action << ", source: " << r->intent.context.source
                      << ", intentId: " << r->intentId << std::endl;
        }
    }
    else if (method == "Actions.start")
    {
        std::string intent = paramFromConsole("intent (JSON)", R"({"action":"pre-load","context":{"source":"system"}})");
        std::string handlerAppIdStr = paramFromConsole("handlerAppId (leave empty to skip)", "");
        std::optional<std::string> handlerAppId;
        if (!handlerAppIdStr.empty())
        {
            handlerAppId = handlerAppIdStr;
        }
        auto r = Firebolt::IFireboltAccessor::Instance().ActionsInterface().start(intent, handlerAppId);
        if (succeed(r))
        {
            std::cout << "Actions.start: Success" << std::endl;
        }
    }
    else if (method == "Actions.onIntent")
    {
        auto callback = [&](const Intent& payload)
        {
            std::cout << "Intent received - action: " << payload.intent.action
                      << ", source: " << payload.intent.context.source << ", intentId: " << payload.intentId
                      << std::endl;
        };
        auto r = Firebolt::IFireboltAccessor::Instance().ActionsInterface().subscribeOnIntent(std::move(callback));
        if (succeed(r))
        {
            std::cout << "Subscribed to Actions.onIntent with Subscription ID: " << *r << std::endl;
        }
    }
    else if (method == "Actions.unsubscribe")
    {
        std::string idStr = paramFromConsole("subscription ID", "0");
        SubscriptionId id = 0;
        try
        {
            id = static_cast<SubscriptionId>(std::stoul(idStr));
        }
        catch (const std::exception&)
        {
        }
        auto r = Firebolt::IFireboltAccessor::Instance().ActionsInterface().unsubscribe(id);
        if (succeed(r))
        {
            std::cout << "Unsubscribed from Actions subscription " << id << std::endl;
        }
    }
    else if (method == "Actions.unsubscribeAll")
    {
        Firebolt::IFireboltAccessor::Instance().ActionsInterface().unsubscribeAll();
        std::cout << "Unsubscribed from all Actions subscriptions" << std::endl;
    }
}
