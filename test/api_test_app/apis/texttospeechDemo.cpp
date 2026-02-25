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
#include "utils.h"
#include <iostream>
#include <string>

using namespace Firebolt;
using namespace Firebolt::TextToSpeech;

TextToSpeechDemo::TextToSpeechDemo()
    : DemoBase("TextToSpeech")
{
    methods_.push_back("TextToSpeech.cancel");
    methods_.push_back("TextToSpeech.getSpeechState");
    methods_.push_back("TextToSpeech.listVoices");
    methods_.push_back("TextToSpeech.pause");
    methods_.push_back("TextToSpeech.resume");
    methods_.push_back("TextToSpeech.speak");
    methods_.push_back("TextToSpeech.onSpeechPause");
    methods_.push_back("TextToSpeech.onSpeechResume");
    methods_.push_back("TextToSpeech.onSpeechStart");
    methods_.push_back("TextToSpeech.onWillSpeak");
}

SpeechId TextToSpeechDemo::chooseSpeechIdFromConsole()
{
    try
    {
        return std::stoi(paramFromConsole("speech ID", std::to_string(speechId_)));
    }
    catch (const std::exception&)
    {
        return 0;
    }
}

void TextToSpeechDemo::runOption(const std::string& method)
{
    std::cout << "Running TextToSpeech method: " << method << std::endl;

    if (method == "TextToSpeech.cancel")
    {
        SpeechId speechId = chooseSpeechIdFromConsole();
        auto r = IFireboltAccessor::Instance().TextToSpeechInterface().cancel(speechId);
        succeed(r);
    }
    else if (method == "TextToSpeech.getSpeechState")
    {
        SpeechId speechId = chooseSpeechIdFromConsole();
        auto r = IFireboltAccessor::Instance().TextToSpeechInterface().getSpeechState(speechId);
        if (succeed(r))
        {
            const SpeechState& state = r->speechState;
            std::cout << "Current state for speech ID '" << speechId << "': " << (int)state << std::endl;
        }
    }
    else if (method == "TextToSpeech.listVoices")
    {
        std::string language = paramFromConsole("language (BCP 47 locale tag)", "en-US");

        auto r = IFireboltAccessor::Instance().TextToSpeechInterface().listVoices(language);
        if (succeed(r))
        {
            const auto& voices = r->voices;
            std::cout << "Voices available for language '" << language << "':" << std::endl;
            for (const auto& voice : voices)
            {
                std::cout << "  Name: " << voice << std::endl;
            }
        }
    }
    else if (method == "TextToSpeech.pause")
    {
        SpeechId speechId = chooseSpeechIdFromConsole();
        auto r = IFireboltAccessor::Instance().TextToSpeechInterface().pause(speechId);
        succeed(r);
    }
    else if (method == "TextToSpeech.resume")
    {
        SpeechId speechId = chooseSpeechIdFromConsole();
        auto r = IFireboltAccessor::Instance().TextToSpeechInterface().resume(speechId);
        succeed(r);
    }
    else if (method == "TextToSpeech.speak")
    {
        std::string text = paramFromConsole("text to speak", "Hello, world!");

        auto r = IFireboltAccessor::Instance().TextToSpeechInterface().speak(text);
        if (succeed(r))
        {
            std::cout << "SpeechResponse: SpeechId: '" << r->speechId << "', TTSStatus: '" << r->ttsStatus << "'  "
                      << std::endl;
            speechId_ = r->speechId;
        }
    }
    else if (method == "TextToSpeech.onSpeechPause")
    {
        auto callback = [&](const SpeechIdEvent& event)
        { std::cout << "Speech Pause notification received for Speech ID: " << event.speechId << std::endl; };
        auto r = IFireboltAccessor::Instance().TextToSpeechInterface().subscribeOnSpeechPause(std::move(callback));
        if (succeed(r))
        {
            std::cout << "Subscribed to onSpeechPause with Subscription ID: " << *r << std::endl;
        }
    }
    else if (method == "TextToSpeech.onSpeechResume")
    {
        auto callback = [&](const SpeechIdEvent& event)
        { std::cout << "Speech Resume notification received for Speech ID: " << event.speechId << std::endl; };
        auto r = IFireboltAccessor::Instance().TextToSpeechInterface().subscribeOnSpeechResume(std::move(callback));
        if (succeed(r))
        {
            std::cout << "Subscribed to onSpeechResume with Subscription ID: " << *r << std::endl;
        }
    }
    else if (method == "TextToSpeech.onSpeechStart")
    {
        auto callback = [&](const SpeechIdEvent& event)
        { std::cout << "Speech Start notification received for Speech ID: " << event.speechId << std::endl; };
        auto r = IFireboltAccessor::Instance().TextToSpeechInterface().subscribeOnSpeechStart(std::move(callback));
        if (succeed(r))
        {
            std::cout << "Subscribed to onSpeechStart with Subscription ID: " << *r << std::endl;
        }
    }
    else if (method == "TextToSpeech.onWillSpeak")
    {
        auto callback = [&](const TextToSpeech::SpeechIdEvent& event)
        { std::cout << "Will Speak notification received for Speech ID: " << event.speechId << std::endl; };
        auto r = IFireboltAccessor::Instance().TextToSpeechInterface().subscribeOnWillSpeak(std::move(callback));
        if (succeed(r))
        {
            std::cout << "Subscribed to onWillSpeak with Subscription ID: " << *r << std::endl;
        }
    }
}
