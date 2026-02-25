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

#include "localizationDemo.h"
#include <firebolt/firebolt.h>
#include <iostream>
#include <string>
#include <vector>

using namespace Firebolt;

LocalizationDemo::LocalizationDemo()
    : DemoBase("Localization")
{
    methods_.push_back("Localization.country");
    methods_.push_back("Localization.preferredAudioLanguages");
    methods_.push_back("Localization.presentationLanguage");
}

void LocalizationDemo::runOption(const std::string& method)
{
    if (method == "Localization.country")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().LocalizationInterface().country();
        if (succeed(r))
        {
            std::cout << "Country: " << *r << std::endl;
        }
    }
    else if (method == "Localization.preferredAudioLanguages")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().LocalizationInterface().preferredAudioLanguages();
        if (succeed(r))
        {
            std::cout << "Preferred Audio Languages: ";
            for (const auto& lang : *r)
            {
                std::cout << lang << " ";
            }
            std::cout << std::endl;
        }
    }
    else if (method == "Localization.presentationLanguage")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().LocalizationInterface().presentationLanguage();
        if (succeed(r))
        {
            std::cout << "Presentation Language: " << *r << std::endl;
        }
    }
}
