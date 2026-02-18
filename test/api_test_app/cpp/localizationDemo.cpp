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
#include <iostream>
#include <string>
#include <vector>

#include "outputstream.h"
extern OutputStream gOutput;

using namespace Firebolt;
using namespace Firebolt::Localization;

LocalizationDemo::LocalizationDemo()
    : FireboltDemoBase()
{
    methodsFromRpc("Localization");
}

void LocalizationDemo::runOption(const int index)
{
    std::string key = itemDescriptions_[index].name;

    if (key == "Localization.country")
    {
        Result<std::string> result = Firebolt::IFireboltAccessor::Instance().LocalizationInterface().country();
        if (validateResult(result))
        {
            gOutput << "Country: " << result.value() << std::endl;
        }
    }
    else if (key == "Localization.preferredAudioLanguages")
    {
        Result<std::vector<std::string>> result =
            Firebolt::IFireboltAccessor::Instance().LocalizationInterface().preferredAudioLanguages();
        if (validateResult(result))
        {
            gOutput << "Preferred Audio Languages: ";
            for (const auto& lang : result.value())
            {
                gOutput << lang << " ";
            }
            gOutput << std::endl;
        }
    }
    else if (key == "Localization.presentationLanguage")
    {
        Result<std::string> result =
            Firebolt::IFireboltAccessor::Instance().LocalizationInterface().presentationLanguage();
        if (validateResult(result))
        {
            gOutput << "Presentation Language: " << result.value() << std::endl;
        }
    }
}
