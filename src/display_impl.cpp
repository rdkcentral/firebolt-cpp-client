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

#include "display_impl.h"
#include "json_types/display.h"

namespace Firebolt::Display
{
DisplayImpl::DisplayImpl(Firebolt::Helpers::IHelper& helper)
    : helper_(helper)
{
}

Result<std::string> DisplayImpl::edid() const
{
    return helper_.get<Firebolt::JSON::String, std::string>("Display.edid");
}

Result<DisplaySize> DisplayImpl::maxResolution() const
{
    return helper_.get<JsonData::DisplaySizeJson, DisplaySize>("Display.maxResolution");
}

Result<DisplaySize> DisplayImpl::size() const
{
    return helper_.get<JsonData::DisplaySizeJson, DisplaySize>("Display.size");
}

Result<std::vector<VideoResolution>> DisplayImpl::videoResolutions() const
{
    return helper_.get<
        Firebolt::JSON::NL_Json_Array<JsonData::VideoResolutionJson, VideoResolution>,
        std::vector<VideoResolution>>("Display.videoResolutions");
}

Result<std::vector<ColorimetryStandard>> DisplayImpl::colorimetry() const
{
    return helper_.get<
        Firebolt::JSON::NL_Json_Array<JsonData::ColorimetryStandardJson, ColorimetryStandard>,
        std::vector<ColorimetryStandard>>("Display.colorimetry");
}
} // namespace Firebolt::Display
