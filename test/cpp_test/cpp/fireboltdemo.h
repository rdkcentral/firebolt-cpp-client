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

#pragma once

#include "firebolt/firebolt.h"
#include "outputstream.h"
#include <firebolt/json_types.h>
#include <iostream>
#include <string>
#include <vector>

class IFireboltDemo
{
public:
    IFireboltDemo();
    virtual ~IFireboltDemo() = default;

    int chooseOption();
    virtual void runOption(const int index) = 0;
    int listSize() const { return static_cast<int>(names_.size()); }
    std::vector<std::string> methods() const { return names_; }

protected:
    void paramFromConsole(const std::string& name, const std::string& def, std::string& value);
    int chooseFromList(const std::vector<std::string>& options, const std::string& prompt);

    void loadRpc();

    template <typename T> bool validateResult(const Firebolt::Result<T>& result) const
    {
        return doValidateResult(result.error());
    }

    bool validateResult(const Firebolt::Result<void>& result) const { return doValidateResult(result.error()); }

    template <typename T> T chooseEnumFromList(const Firebolt::JSON::EnumType<T>& enumType, const std::string& prompt)
    {
        std::vector<std::string> options;
        for (const auto& pair : enumType)
        {
            options.push_back(pair.first);
            std::cout << "Option: " << pair.first << std::endl;
        }

        int choice = chooseFromList(options, prompt);

        return enumType.at(options[choice]);
    }

    template <typename T> std::string stringFromEnum(Firebolt::JSON::EnumType<T> enumType, T value)
    {
        for (const auto& pair : enumType)
        {
            if (pair.second == value)
            {
                return pair.first;
            }
        }
        return {};
    }

    std::vector<std::string> methodsFromRpc(const std::string& interfaceName);

    static nlohmann::json json_;

    std::vector<std::string> names_;
    std::vector<std::string> descriptions_;

    OutputStream outStream_;

private:
    bool doValidateResult(Firebolt::Error error) const
    {
        if (error != Firebolt::Error::None)
        {
            std::cout << "Operation failed with error: " << static_cast<int>(error) << std::endl;
            return false;
        }
        std::cout << "Operation succeeded." << std::endl;
        return true;
    }
};
