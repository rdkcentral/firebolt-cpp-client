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

#pragma once

#include <firebolt/json_types.h>
#include <firebolt/types.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

struct AppConfig
{
    bool autoRun = false;
};

AppConfig& GetAppConfig();

std::string paramFromConsole(const std::string& name, const std::string& def);
int chooseFromList(const std::vector<std::string>& options, const std::string& prompt);

class DemoBase
{
public:
    DemoBase(const std::string& name)
        : name_(name)
    {
    }
    virtual ~DemoBase() = default;

    std::string name() const { return name_; }
    virtual void runOption(const std::string& method) = 0;
    const std::vector<std::string>& methods() const { return methods_; }

protected:
    template <typename T> bool succeed(const Firebolt::Result<T>& result) const
    {
        if (result)
        {
            return true;
        }
        std::cout << "Error: " << static_cast<int>(result.error()) << std::endl;
        return false;
    }

    std::string name_;
    std::vector<std::string> methods_;
};

template <typename T> T chooseEnumFromList(const Firebolt::JSON::EnumType<T>& enumType, const std::string& prompt)
{
    if (enumType.empty())
    {
        throw std::runtime_error("EnumType cannot be empty");
    }
    if (GetAppConfig().autoRun)
    {
        std::cout << "Auto-selecting '" << enumType.begin()->first << "' for " << prompt << "." << std::endl;
        return enumType.begin()->second;
    }

    std::vector<std::string> options;
    for (const auto& pair : enumType)
    {
        options.push_back(pair.first);
    }

    int choice = chooseFromList(options, prompt);
    if (choice == -1)
    {
        choice = 0;
    }

    return enumType.at(options[choice]);
}
