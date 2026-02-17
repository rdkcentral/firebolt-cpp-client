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

#include "fireboltdemo.h"
#include "firebolt-open-rpc_json.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "outputstream.h"
extern OutputStream gOutput;


nlohmann::json IFireboltDemo::json_;

#ifndef UT_OPEN_RPC_FILE
#define UT_OPEN_RPC_FILE "firebolt-open-rpc.json"
#endif

IFireboltDemo::IFireboltDemo()
{
    loadRpc();
}

void IFireboltDemo::paramFromConsole(const std::string& name, const std::string& def, std::string& value)
{
    if (gAutoRun)
    {
        gOutput << "Auto-selecting " << def << " for " << name << "." << std::endl;
        value = def;
        return;
    }
    gOutput << "Enter " << name << " (default: " << def << "): ";
    std::string input;
    std::getline(std::cin, input);
    if (input.empty())
    {
        value = def;
    }
    else
    {
        value = input;
    }
}

int getOption(int n)
{
    std::string input;

    while (true)
    {
        std::cout << "Select option or 'q' to go back: ";
        std::cin >> input;

        // Check if user wants to quit
        if (input.length() == 1 && std::tolower(input[0]) == 'q')
        {
            return -1;
        }

        // Try to convert to integer
        try
        {
            int num = std::stoi(input);

            // Check if number is in valid range
            if (num >= 0 && num <= n)
            {
                return num;
            }
            else
            {
                gOutput << "Please enter a number between 0 and " << n << "." << std::endl;
            }
        }
        catch (const std::invalid_argument&)
        {
            std::cout << "Invalid input. Please enter a number or 'q'." << std::endl;
        }
        catch (const std::out_of_range&)
        {
            std::cout << "Number too large. Please enter a number between 0 and " << n << "." << std::endl;
        }
    }
}

int IFireboltDemo::chooseFromList(const std::vector<std::string>& options, const std::string& prompt)
{
    std::cout << prompt << std::endl;
    for (size_t i = 0; i < options.size(); ++i)
    {
        std::cout << i << ": " << options[i] << std::endl;
    }

    int choice = getOption(static_cast<int>(options.size() - 1));

    return choice;
}

int IFireboltDemo::chooseOption()
{
    std::vector<std::string> methodNames;
    for (const auto& item : itemDescriptions_)    {
        methodNames.push_back(item.name);
    }
    return chooseFromList(methodNames, "Choose a method to run:");
}

void IFireboltDemo::loadRpc()
{
    json_ = nlohmann::json::parse(JSON_DATA);
}

std::vector<std::string> IFireboltDemo::methodsFromRpc(const std::string& interfaceName)
{
    std::vector<std::string> methodNames;

    std::string interfaceStr = interfaceName + ".";
    for (const auto& method : json_["methods"])
    {
        if (method.contains("name") && method["name"].get<std::string>().rfind(interfaceStr, 0) == 0)
        {
            itemDescriptions_.push_back({method["name"], method["summary"]});
           // gOutput << "Found method: " << names_.back() << ":" << descriptions_.back() << std::endl;
        }
    }

    return methodNames;
}
