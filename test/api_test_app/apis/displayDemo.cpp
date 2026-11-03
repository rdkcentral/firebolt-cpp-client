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

#include "displayDemo.h"
#include <firebolt/firebolt.h>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

using namespace Firebolt;
using namespace Firebolt::Display;

DisplayDemo::DisplayDemo()
    : DemoBase("Display")
{
    methods_.push_back("Display.connected");
    methods_.push_back("Display.edid");
    methods_.push_back("Display.maxResolution");
    methods_.push_back("Display.size");
}

void DisplayDemo::runOption(const std::string& method)
{
    std::cout << "Running Display method: " << method << std::endl;
    if (method == "Display.connected")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().DisplayInterface().connected();
        if (succeed(r))
        {
            std::cout << "Display Connected: " << std::boolalpha << *r << std::endl;
        }
    }
    else if (method == "Display.edid")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().DisplayInterface().edid();
        if (succeed(r))
        {
            std::cout << "EDID: " << *r << std::endl;
        }
    }
    else if (method == "Display.maxResolution")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().DisplayInterface().maxResolution();
        if (succeed(r))
        {
            std::cout << "Max Resolution - Width: " << r->width << ", Height: " << r->height << std::endl;
        }
    }
    else if (method == "Display.size")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().DisplayInterface().size();
        if (succeed(r))
        {
            std::cout << "Display Size - Width: " << r->width << ", Height: " << r->height << std::endl;
        }
    }
}
