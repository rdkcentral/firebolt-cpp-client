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

#include "chooseInterface.h"
#include "fireboltDemoBase.h"

#include "accessibilityDemo.h"
#include "advertisingDemo.h"
#include "deviceDemo.h"
#include "discoveryDemo.h"
#include "displayDemo.h"
#include "lifecycleDemo.h"
#include "localizationDemo.h"
#include "presentationDemo.h"
#include "statsDemo.h"

#include <iostream>

#include "outputstream.h"
extern OutputStream gOutput;

ChooseInterface::ChooseInterface()
    : FireboltDemoBase()
{
    for (auto& [key, value] : json_["info"]["x-module-descriptions"].items())
    {
        std::cout << key << ": " << value << std::endl;
        if (key != "Internal")
        {
            itemDescriptions_.push_back({key, value.get<std::string>()});
            std::cout << "Added interface: " << key << std::endl;
        }
    }

    interfaces.clear();
    interfaces.emplace_back(std::make_unique<AccessibilityDemo>());
    interfaces.emplace_back(std::make_unique<AdvertisingDemo>());
    interfaces.emplace_back(std::make_unique<DeviceDemo>());
    interfaces.emplace_back(std::make_unique<DiscoveryDemo>());
    interfaces.emplace_back(std::make_unique<DisplayDemo>());
    interfaces.emplace_back(std::make_unique<LifecycleDemo>());
    interfaces.emplace_back(std::make_unique<LocalizationDemo>());
    interfaces.emplace_back(std::make_unique<PresentationDemo>());
    interfaces.emplace_back(std::make_unique<StatsDemo>());

    std::cout << interfaces.size() << " interfaces, " << itemDescriptions_.size() << " descriptions" << std::endl;
    assert(interfaces.size() == itemDescriptions_.size());
}

ChooseInterface::~ChooseInterface() = default;

void ChooseInterface::runOption(const int index)
{
    FireboltDemoBase* selectedInterface = interfaces[index].get();
    gOutput << "Running interface: " << itemDescriptions_[index].name << std::endl;

    for (;;)
    {
        int methodIndex = selectedInterface->chooseOption();

        if (methodIndex == -1)
        {
            break; // Go back to interface selection
        }

        selectedInterface->runOption(methodIndex);
    }
}

void ChooseInterface::autoRun()
{
    for (int i = 0; i < (int)interfaces.size(); ++i)
    {
        FireboltDemoBase* selectedInterface = interfaces[i].get();
        if (selectedInterface == nullptr)
        {
            assert(("Interface not implemented for: " + itemDescriptions_[i].name).c_str());
        }
        gOutput << "Auto-running interface: " << itemDescriptions_[i].name << std::endl;

        // Assuming each interface has a predefined set of methods to run
        for (int j = 0; j < selectedInterface->listSize(); ++j)
        {
            gOutput << "Auto-running method: " << selectedInterface->method(j) << std::endl;
            selectedInterface->runOption(j);
        }
    }
}
