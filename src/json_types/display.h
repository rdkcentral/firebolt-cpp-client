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

#pragma once

#include "firebolt/display.h"
#include <firebolt/json_types.h>
#include <nlohmann/json.hpp>

namespace Firebolt::Display::JsonData
{

// Types
class DisplaySizeJson : public Firebolt::JSON::NL_Json_Basic<::Firebolt::Display::DisplaySize>
{
public:
    void fromJson(const nlohmann::json& json) override
    {
        if (!checkRequiredFields(json, {"width", "height"}))
        {
            throw std::invalid_argument("Missing required fields in JSON");
        }
        width_ = json["width"].get<uint32_t>();
        height_ = json["height"].get<uint32_t>();
    }
    ::Firebolt::Display::DisplaySize value() const override { return Firebolt::Display::DisplaySize{width_, height_}; }

private:
    uint32_t width_;
    uint32_t height_;
};


inline const Firebolt::JSON::EnumType<::Firebolt::Display::VideoResolution> VideoResolutionEnum({
    {"720p50",  ::Firebolt::Display::VideoResolution::R720p50},
    {"720p60",  ::Firebolt::Display::VideoResolution::R720p60},
    {"1080p50", ::Firebolt::Display::VideoResolution::R1080p50},
    {"1080p60", ::Firebolt::Display::VideoResolution::R1080p60},
    {"2160p50", ::Firebolt::Display::VideoResolution::R2160p50},
    {"2160p60", ::Firebolt::Display::VideoResolution::R2160p60},
});

class VideoResolutionJson : public Firebolt::JSON::NL_Json_Basic<::Firebolt::Display::VideoResolution>
{
public:
    void fromJson(const nlohmann::json& json) override { value_ = VideoResolutionEnum.at(json); }
    ::Firebolt::Display::VideoResolution value() const override { return value_; }
private:
    ::Firebolt::Display::VideoResolution value_{};
};

inline const Firebolt::JSON::EnumType<::Firebolt::Display::ColorimetryStandard> ColorimetryStandardEnum({
    {"bt709",  ::Firebolt::Display::ColorimetryStandard::Bt709},
    {"bt2020", ::Firebolt::Display::ColorimetryStandard::Bt2020},
});

class ColorimetryStandardJson : public Firebolt::JSON::NL_Json_Basic<::Firebolt::Display::ColorimetryStandard>
{
public:
    void fromJson(const nlohmann::json& json) override { value_ = ColorimetryStandardEnum.at(json); }
    ::Firebolt::Display::ColorimetryStandard value() const override { return value_; }
private:
    ::Firebolt::Display::ColorimetryStandard value_{};
};

} // namespace Firebolt::Display::JsonData
