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

#include "firebolt/texttospeech.h"
#include <firebolt/json_types.h>

namespace Firebolt::TextToSpeech::JsonData
{
inline const Firebolt::JSON::EnumType<Firebolt::TextToSpeech::SpeechRate> SpeechRateEnum({
    {"slow", ::Firebolt::TextToSpeech::SpeechRate::SLOW},
    {"medium", ::Firebolt::TextToSpeech::SpeechRate::MEDIUM},
    {"fast", ::Firebolt::TextToSpeech::SpeechRate::FAST},
    {"faster", ::Firebolt::TextToSpeech::SpeechRate::FASTER},
    {"fastest", ::Firebolt::TextToSpeech::SpeechRate::FASTEST},
});

class ListVoicesResponse : public Firebolt::JSON::NL_Json_Basic<::Firebolt::TextToSpeech::ListVoicesResponse>
{
public:
    void fromJson(const nlohmann::json& json) override
    {
        ttsStatus_ = json["TTS_Status"].get<int32_t>();
        voices_.clear();
        for (const auto& voice : json["voices"])
        {
            voices_.push_back(voice.get<std::string>());
        }
    }
    ::Firebolt::TextToSpeech::ListVoicesResponse value() const override
    {
        return ::Firebolt::TextToSpeech::ListVoicesResponse{ttsStatus_, voices_};
    }

private:
    int32_t ttsStatus_;
    std::vector<std::string> voices_;
};

class SpeechIdEvent : public Firebolt::JSON::NL_Json_Basic<::Firebolt::TextToSpeech::SpeechIdEvent>
{
public:
    void fromJson(const nlohmann::json& json) override { speechId_ = json["speechid"].get<int32_t>(); }
    ::Firebolt::TextToSpeech::SpeechIdEvent value() const override
    {
        return ::Firebolt::TextToSpeech::SpeechIdEvent{speechId_};
    }

private:
    int32_t speechId_;
};

class SpeechResponse : public Firebolt::JSON::NL_Json_Basic<::Firebolt::TextToSpeech::SpeechResponse>
{
public:
    void fromJson(const nlohmann::json& json) override
    {
        speechId_ = json["speechid"].get<int32_t>();
        ttsStatus_ = json["TTS_Status"].get<int32_t>();
        success_ = json["success"].get<bool>();
    }
    ::Firebolt::TextToSpeech::SpeechResponse value() const override
    {
        return ::Firebolt::TextToSpeech::SpeechResponse{speechId_, ttsStatus_, success_};
    }

private:
    int32_t speechId_;
    int32_t ttsStatus_;
    bool success_;
};

class SpeechStateResponse : public Firebolt::JSON::NL_Json_Basic<::Firebolt::TextToSpeech::SpeechStateResponse>
{
public:
    void fromJson(const nlohmann::json& json) override
    {
        speechState_ = static_cast<SpeechState>(json["speechstate"].get<int>());
        ttsStatus_ = json["TTS_Status"].get<int32_t>();
        success_ = json["success"].get<bool>();
    }
    ::Firebolt::TextToSpeech::SpeechStateResponse value() const override
    {
        return ::Firebolt::TextToSpeech::SpeechStateResponse{speechState_, ttsStatus_, success_};
    }

private:
    SpeechState speechState_;
    int32_t ttsStatus_;
    bool success_;
};

class TTSStatusResponse : public Firebolt::JSON::NL_Json_Basic<::Firebolt::TextToSpeech::TTSStatusResponse>
{
public:
    void fromJson(const nlohmann::json& json) override
    {
        ttsStatus_ = json["TTS_Status"].get<int32_t>();
        success_ = json["success"].get<bool>();
    }
    ::Firebolt::TextToSpeech::TTSStatusResponse value() const override
    {
        return ::Firebolt::TextToSpeech::TTSStatusResponse{ttsStatus_, success_};
    }

private:
    int32_t ttsStatus_;
    bool success_;
};

} // namespace Firebolt::TextToSpeech::JsonData
