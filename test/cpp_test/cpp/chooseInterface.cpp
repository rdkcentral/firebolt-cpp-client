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
#include "fireboltdemo.h"

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

    interfaces = {new AccessibilityDemo(), new AdvertisingDemo(),  new DeviceDemo(),
                  new DiscoveryDemo(),     new DisplayDemo(),      new LifecycleDemo(),
                  new LocalizationDemo(),  new PresentationDemo(), new StatsDemo()};

    std::cout << interfaces.size() << " interfaces, " << itemDescriptions_.size() << " descriptions" << std::endl;
    assert(interfaces.size() == itemDescriptions_.size());
}

ChooseInterface::~ChooseInterface()
{
    for (auto interfacePtr : interfaces)
    {
        delete interfacePtr;
    }
}

void ChooseInterface::runOption(const int index)
{
    FireboltDemoBase* selectedInterface = interfaces[index];
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
        FireboltDemoBase* selectedInterface = interfaces[i];
        if (selectedInterface == nullptr)
        {
            continue; // Skip unimplemented interfaces
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
