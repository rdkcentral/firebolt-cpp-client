/*
 * If not stated otherwise in this file or this component's LICENSE file the
 * following copyright and licenses apply:
 *
 * Copyright 2025 Sky UK
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

class FallbackText : public Firebolt::JSON::NL_Json_Basic<::Firebolt::TextToSpeech::FallbackText>
{
public:
    void fromJson(const nlohmann::json& json) override
    {
        if (json.contains("scenario"))
        {
            scenario_ = json["scenario"].get<std::string>();
        }
        if (json.contains("value"))
        {
            value_ = json["value"].get<std::string>();
        }
    }
    ::Firebolt::TextToSpeech::FallbackText value() const override
    {
        ::Firebolt::TextToSpeech::FallbackText fallbackText;
        if (scenario_.has_value())
        {
            fallbackText.scenario = scenario_.value();
        }
        if (value_.has_value())
        {
            fallbackText.value = value_.value();
        }
        return fallbackText;
    }

private:
    std::optional<std::string> scenario_;
    std::optional<std::string> value_;
};

class ListVoicesResponse : public Firebolt::JSON::NL_Json_Basic<::Firebolt::TextToSpeech::ListVoicesResponse>
{
public:
    void fromJson(const nlohmann::json& json) override
    {
        ttsStatus_ = json["TTS_Status"];
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
    void fromJson(const nlohmann::json& json) override
    {
        speechId_ = json["speechid"].get<int32_t>();
        if (json.contains("text"))
        {
            text_ = json["text"].get<std::string>();
        }
    }
    ::Firebolt::TextToSpeech::SpeechIdEvent value() const override
    {
        return ::Firebolt::TextToSpeech::SpeechIdEvent{speechId_, text_};
    }

private:
    int32_t speechId_;
    std::optional<std::string> text_;
};

class SpeechResponse : public Firebolt::JSON::NL_Json_Basic<::Firebolt::TextToSpeech::SpeechResponse>
{
public:
    void fromJson(const nlohmann::json& json) override
    {
        speechId_ = json["speechid"];
        ttsStatus_ = json["TTS_Status"];
        success_ = json["success"];
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
        speechState_ = std::to_string(json["speechstate"].get<int>());
        ttsStatus_ = json["TTS_Status"];
        success_ = json["success"];
    }
    ::Firebolt::TextToSpeech::SpeechStateResponse value() const override
    {
        return ::Firebolt::TextToSpeech::SpeechStateResponse{speechState_, ttsStatus_, success_};
    }

private:
    std::string speechState_;
    int32_t ttsStatus_;
    bool success_;
};

class TTSStatusResponse : public Firebolt::JSON::NL_Json_Basic<::Firebolt::TextToSpeech::TTSStatusResponse>
{
public:
    void fromJson(const nlohmann::json& json) override
    {
        ttsStatus_ = json["TTS_Status"];
        success_ = json["success"];
    }
    ::Firebolt::TextToSpeech::TTSStatusResponse value() const override
    {
        return ::Firebolt::TextToSpeech::TTSStatusResponse{ttsStatus_, success_};
    }

private:
    int32_t ttsStatus_;
    bool success_;
};

class TTSState : public Firebolt::JSON::NL_Json_Basic<::Firebolt::TextToSpeech::TTSState>
{
public:
    void fromJson(const nlohmann::json& json) override { state_ = json["state"].get<bool>(); }
    ::Firebolt::TextToSpeech::TTSState value() const override { return ::Firebolt::TextToSpeech::TTSState{state_}; }

private:
    bool state_;
};

class TTSVoice : public Firebolt::JSON::NL_Json_Basic<::Firebolt::TextToSpeech::TTSVoice>
{
public:
    void fromJson(const nlohmann::json& json) override { voice_ = json["voice"]; }
    ::Firebolt::TextToSpeech::TTSVoice value() const override { return ::Firebolt::TextToSpeech::TTSVoice{voice_}; }

private:
    std::string voice_;
};
} // namespace Firebolt::TextToSpeech::JsonData
