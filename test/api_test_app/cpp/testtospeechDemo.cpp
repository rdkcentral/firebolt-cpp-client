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

#include "texttospeechDemo.h"
#include <iostream>
#include <string>
#include <vector>

#include "json_types/jsondata_texttospeech_types.h"

using namespace Firebolt;
using namespace Firebolt::TextToSpeech;

TextToSpeechDemo::TextToSpeechDemo()
    : FireboltDemoBase()
{
    methodsFromRpc("TextToSpeech");
}

SpeechId TextToSpeechDemo::chooseSpeechIdFromConsole()
{
    std::string sId;
    ;
    std::string savedId = std::to_string(speechId_);

    paramFromConsole("speech ID to pause", savedId, sId);
    SpeechId speechId = std::stoi(sId);
    return speechId;
}

void TextToSpeechDemo::runOption(const int index)
{
    std::string key = itemDescriptions_[index].name;

    std::cout << "Running TextToSpeech method: " << key << std::endl;

    if (key == "TextToSpeech.speak")
    {
        std::string text;
        paramFromConsole("text to speak", "Hello, world!", text);

        Result<SpeechResponse> r = IFireboltAccessor::Instance().TextToSpeechInterface().speak(text);
        validateResult(r);
        std::cout << "SpeechResponse: SpeechId'" << r.value().speechId << " TTSStatus'" << r.value().ttsStatus << "'  "
                  << std::endl;
        speechId_ = r.value().speechId;
    }
    else if (key == "TextToSpeech.listVoices")
    {
        std::string language;
        paramFromConsole("language (BCP 47 locale tag)", "en-US", language);

        Result<ListVoicesResponse> r = IFireboltAccessor::Instance().TextToSpeechInterface().listVoices(language);
        if (validateResult(r))
        {
            const auto& voices = r.value().voices;
            std::cout << "Voices available for language '" << language << "':" << std::endl;
            for (const auto& voice : voices)
            {
                std::cout << "  Name: " << voice << std::endl;
            }
        }
    }
    else if (key == "TextToSpeech.pause")
    {
        SpeechId speechId = chooseSpeechIdFromConsole();
        Result<TTSStatusResponse> r = IFireboltAccessor::Instance().TextToSpeechInterface().pause(speechId);
        validateResult(r);
    }
    else if (key == "TextToSpeech.resume")
    {
        SpeechId speechId = chooseSpeechIdFromConsole();
        Result<TTSStatusResponse> r = IFireboltAccessor::Instance().TextToSpeechInterface().resume(speechId);
        validateResult(r);
    }
    else if (key == "TextToSpeech.cancel")
    {
        SpeechId speechId = chooseSpeechIdFromConsole();
        Result<TTSStatusResponse> r = IFireboltAccessor::Instance().TextToSpeechInterface().cancel(speechId);
        validateResult(r);
    }
    else if (key == "TextToSpeech.getSpeechState")
    {
        SpeechId speechId = chooseSpeechIdFromConsole();
        Result<SpeechStateResponse> r = IFireboltAccessor::Instance().TextToSpeechInterface().getSpeechState(speechId);
        if (validateResult(r))
        {
            const SpeechState& state = r.value().speechState;
            std::cout << "Current state for speech ID '" << speechId << "': " << (int)state << std::endl;
        }
    }
    else if (key == "TextToSpeech.speechRate")
    {
        SpeechId speechId = chooseSpeechIdFromConsole();
        Result<SpeechStateResponse> r = IFireboltAccessor::Instance().TextToSpeechInterface().getSpeechState(speechId);
        if (validateResult(r))
        {
            const auto& ttsStatus = r.value().ttsStatus;
            std::cout << "Current TTS status for speech ID '" << speechId << "': " << ttsStatus << std::endl;
        }
    }
    else if (key == "TextToSpeech.subscribeOnWillSpeak")
    {
        auto callback = [&](const TextToSpeech::SpeechIdEvent& event)
        { std::cout << "Will Speak notification received for Speech ID: " << event.speechId << std::endl; };
        Result<SubscriptionId> r =
            IFireboltAccessor::Instance().TextToSpeechInterface().subscribeOnWillSpeak(std::move(callback));
        if (validateResult(r))
        {
            std::cout << "Subscribed to OnWillSpeak with Subscription ID: " << r.value() << std::endl;
        }
    }
    else if (key == "TextToSpeech.subscribeOnSpeechStart")
    {
        auto callback = [&](const SpeechIdEvent& event)
        { std::cout << "Speech Start notification received for Speech ID: " << event.speechId << std::endl; };
        Result<SubscriptionId> r =
            IFireboltAccessor::Instance().TextToSpeechInterface().subscribeOnSpeechStart(std::move(callback));
        if (validateResult(r))
        {
            std::cout << "Subscribed to OnSpeechStart with Subscription ID: " << r.value() << std::endl;
        }
    }
    else if (key == "TextToSpeech.subscribeOnSpeechPause")
    {
        auto callback = [&](const SpeechIdEvent& event)
        { std::cout << "Speech Pause notification received for Speech ID: " << event.speechId << std::endl; };
        Result<SubscriptionId> r =
            IFireboltAccessor::Instance().TextToSpeechInterface().subscribeOnSpeechPause(std::move(callback));
        if (validateResult(r))
        {
            std::cout << "Subscribed to OnSpeechPause with Subscription ID: " << r.value() << std::endl;
        }
    }
    else if (key == "TextToSpeech.subscribeOnSpeechResume")
    {
        auto callback = [&](const SpeechIdEvent& event)
        { std::cout << "Speech Resume notification received for Speech ID: " << event.speechId << std::endl; };
        Result<SubscriptionId> r =
            IFireboltAccessor::Instance().TextToSpeechInterface().subscribeOnSpeechResume(std::move(callback));
        if (validateResult(r))
        {
            std::cout << "Subscribed to OnSpeechResume with Subscription ID: " << r.value() << std::endl;
        }
    }
}
