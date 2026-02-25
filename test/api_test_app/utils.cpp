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

#include "utils.h"

#include <strings.h>

static AppConfig gAppConfig;

AppConfig& GetAppConfig()
{
    return gAppConfig;
}

std::string paramFromConsole(const std::string& name, const std::string& def)
{
    if (GetAppConfig().autoRun)
    {
        std::cout << "Auto-selecting " << def << " for " << name << "." << std::endl;
        return def;
    }

    std::cout << "Enter " << name << " (default: " << def << "): ";

    std::string input;
    std::getline(std::cin, input);

    if (input.empty())
    {
        return def;
    }
    return input;
}

int getOption(int max)
{
    std::string input;

    while (true)
    {
        std::cout << "Select option or enter/q to go back: ";
        std::getline(std::cin, input);

        if (input.length() == 0 || strcasecmp(input.c_str(), "q") == 0)
        {
            return -1;
        }

        try
        {
            int num = std::stoi(input);

            if (num > 0 && num <= max)
            {
                return num;
            }
            else
            {
                std::cout << "Please enter a number between 1 and " << max << " (inclusive)." << std::endl;
            }
        }
        catch (const std::invalid_argument&)
        {
            std::cout << "Invalid input. Please enter a number or 'q'." << std::endl;
        }
        catch (const std::out_of_range&)
        {
            std::cout << "Number too large. Please enter a number between 1 and " << max << " (inclusive)." << std::endl;
        }
    }
}

int chooseFromList(const std::vector<std::string>& options, const std::string& prompt)
{
    if (options.empty())
    {
        return -1;
    }
    std::cout << std::endl << prompt << std::endl;
    for (size_t i = 0; i < options.size(); ++i)
    {
        std::cout << (i + 1) << ": " << options[i] << std::endl;
    }

    int choice = getOption(options.size());
    if (choice == -1)
    {
        return -1;
    }

    return choice - 1;
}
