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

#include "fireboltDemoBase.h"
#include "firebolt-open-rpc_json.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

nlohmann::json FireboltDemoBase::json_;

#ifndef UT_OPEN_RPC_FILE
#define UT_OPEN_RPC_FILE "firebolt-open-rpc.json"
#endif

FireboltDemoBase::FireboltDemoBase()
{
    loadRpc();
}

int FireboltDemoBase::chooseFromList(const std::vector<std::string>& options, const std::string& prompt, bool allowCancel)
{
    std::cout << std::endl << prompt << std::endl;
    for (size_t i = 0; i < options.size(); ++i)
    {
        std::cout << i << ": " << options[i] << std::endl;
    }

    int choice = getOption(static_cast<int>(options.size() - 1), allowCancel);

    return choice;
}

int FireboltDemoBase::chooseOption()
{
    std::vector<std::string> methodNames;
    for (const auto& item : itemDescriptions_)
    {
        methodNames.push_back(item.name);
    }
    return chooseFromList(methodNames, "Choose a method to run:", true);
}

void FireboltDemoBase::loadRpc()
{
    json_ = nlohmann::json::parse(JSON_DATA);
}

void FireboltDemoBase::methodsFromRpc(const std::string& interfaceName)
{
    std::vector<std::string> methodNames;

    std::string interfaceStr = interfaceName + ".";
    for (const auto& method : json_["methods"])
    {
        if (method.contains("name") && method["name"].get<std::string>().rfind(interfaceStr, 0) == 0)
        {
            itemDescriptions_.push_back({method["name"], method["summary"]});
        }
    }
}
