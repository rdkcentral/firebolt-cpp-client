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

#include "accessibilityDemo.h"
#include <firebolt/firebolt.h>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

using namespace Firebolt;
using namespace Firebolt::Accessibility;

AccessibilityDemo::AccessibilityDemo()
    : DemoBase("Accessibility")
{
    methods_.push_back("Accessibility.audioDescription");
    methods_.push_back("Accessibility.closedCaptionsSettings");
    methods_.push_back("Accessibility.highContrastUI");
    methods_.push_back("Accessibility.voiceGuidanceSettings");
}

void AccessibilityDemo::runOption(const std::string& method)
{
    std::cout << "Running Accessibility method: " << method << std::endl;

    if (method == "Accessibility.closedCaptionsSettings")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().AccessibilityInterface().closedCaptionsSettings();
        if (succeed(r))
        {
            std::cout << "Closed Captions Settings - Enabled: " << std::boolalpha << r->enabled << std::endl;
        }
    }
    else if (method == "Accessibility.audioDescription")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().AccessibilityInterface().audioDescription();
        if (succeed(r))
        {
            std::cout << "Audio Description Enabled: " << std::boolalpha << *r << std::endl;
        }
    }
    else if (method == "Accessibility.highContrastUI")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().AccessibilityInterface().highContrastUI();
        if (succeed(r))
        {
            std::cout << "High Contrast UI Enabled: " << std::boolalpha << *r << std::endl;
        }
    }
    else if (method == "Accessibility.voiceGuidanceSettings")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().AccessibilityInterface().voiceGuidanceSettings();
        if (succeed(r))
        {
            std::cout << "Voice Guidance Settings - Enabled: " << std::boolalpha << r->enabled << ", Rate: " << r->rate
                      << std::endl;
        }
    }
}
