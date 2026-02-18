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
#include <iostream>
#include <string>
#include <vector>

#include "json_types/jsondata_device_types.h"

#include "outputstream.h"
extern OutputStream gOutput;

using namespace Firebolt;
using namespace Firebolt::Display;

DisplayDemo::DisplayDemo()
    : FireboltDemoBase()
{
    methodsFromRpc("Display");
}

void DisplayDemo::runOption(const int index)
{
    std::string key = itemDescriptions_[index].name;

    outStream_ << "Running Display method: " << key << std::endl;
    if (key == "Display.maxResolution")
    {
        Result<DisplaySize> r = Firebolt::IFireboltAccessor::Instance().DisplayInterface().maxResolution();
        if (validateResult(r))
        {
            DisplaySize res = r.value();
            gOutput << "Max Resolution - Width: " << res.width << ", Height: " << res.height << std::endl;
        }
    }
    else if (key == "Display.size")
    {
        Result<DisplaySize> r = Firebolt::IFireboltAccessor::Instance().DisplayInterface().size();
        if (validateResult(r))
        {
            DisplaySize size = r.value();
            gOutput << "Display Size - Width: " << size.width << ", Height: " << size.height << std::endl;
        }
    }
}
