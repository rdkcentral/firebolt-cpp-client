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
#include <iostream>
#include <string>
#include <vector>

#include "json_types/jsondata_device_types.h"

using namespace Firebolt;
using namespace Firebolt::Accessibility;

AccessibilityDemo::AccessibilityDemo()
    : FireboltDemoBase()
{
    methodsFromRpc("Accessibility");
}

void AccessibilityDemo::runOption(const int index)
{
    std::string key = itemDescriptions_[index].name;

    std::cout << "Running Accessibility method: " << key << std::endl;

    if (key == "Accessibility.closedCaptionsSettings")
    {
        Result<ClosedCaptionsSettings> r =
            Firebolt::IFireboltAccessor::Instance().AccessibilityInterface().closedCaptionsSettings();
        if (validateResult(r))
        {
            ClosedCaptionsSettings settings = r.value();
            std::cout << "Closed Captions Settings - Enabled: " << (settings.enabled ? "true" : "false") << std::endl;
        }
    }
    else if (key == "Accessibility.audioDescription")
    {
        Result<bool> r = Firebolt::IFireboltAccessor::Instance().AccessibilityInterface().audioDescription();
        if (validateResult(r))
        {
            bool enabled = r.value();
            std::cout << "Audio Description Enabled: " << (enabled ? "true" : "false") << std::endl;
        }
    }
    else if (key == "Accessibility.highContrastUI")
    {
        Result<bool> r = Firebolt::IFireboltAccessor::Instance().AccessibilityInterface().highContrastUI();
        if (validateResult(r))
        {
            bool enabled = r.value();

            std::cout << "High Contrast UI Enabled: " << (enabled ? "true" : "false") << std::endl;
        }
    }
    else if (key == "Accessibility.voiceGuidanceSettings")
    {
        Result<VoiceGuidanceSettings> r =
            Firebolt::IFireboltAccessor::Instance().AccessibilityInterface().voiceGuidanceSettings();
        if (validateResult(r))
        {
            VoiceGuidanceSettings settings = r.value();
            std::cout << "Voice Guidance Settings - Enabled: " << (settings.enabled ? "true" : "false")
                      << ", Rate: " << settings.rate << std::endl;
        }
    }
}