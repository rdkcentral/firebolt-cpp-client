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
#include "firebolt/firebolt.h"
#include "json_engine.h"
#include "utils.h"

class TextToSpeechTest : public ::testing::Test
{
protected:
    JsonEngine jsonEngine;
};

TEST_F(TextToSpeechTest, isTtsEnabled)
{
    auto ttsEnabled = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().isttsenabled();
    ASSERT_TRUE(ttsEnabled) << "Error getting TTS enabled status";

    auto expectedValue = jsonEngine.get_value("TextToSpeech.isttsenabled");
    EXPECT_EQ(ttsEnabled->TTS_status, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(ttsEnabled->isenabled, expectedValue["isenabled"].get<bool>());
}

TEST_F(TextToSpeechTest, listVoices)
{
    auto voices = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().listvoices("en-US");
    ASSERT_TRUE(voices) << "Error listing voices";

    auto expectedValue = jsonEngine.get_value("TextToSpeech.listvoices");
    EXPECT_EQ(voices->TTS_status, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(voices->voices.size(), expectedValue["voices"].size());
}

TEST_F(TextToSpeechTest, getTtsConfiguration)
{
    auto ttsConfiguration =
        Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().getttsconfiguration();
    ASSERT_TRUE(ttsConfiguration) << "Error getting TTS configuration";

    auto expectedValue = jsonEngine.get_value("TextToSpeech.getttsconfiguration");
    EXPECT_EQ(*ttsConfiguration, expectedValue);
}

TEST_F(TextToSpeechTest, setTtsConfiguration)
{
    Firebolt::TextToSpeech::TTSConfiguration config;
    config.ttsendpoint = "http://test-tts-server.com";
    config.language = "en-US";
    config.voice = "carol";
    config.volume = 50;

    auto result = Firebolt::IFireboltAccessor::Instance()
                      .TextToSpeechInterface()
                      .setttsconfiguration("http://test-tts-server.com", std::nullopt, "en-US", "carol", 50,
                                           std::nullopt, std::nullopt, std::nullopt, std::nullopt);
    ASSERT_TRUE(result) << "Error setting TTS configuration";
}

TEST_F(TextToSpeechTest, speak)
{
    auto speakResult = Firebolt::IFireboltAccessor::Instance()
                           .TextToSpeechInterface()
                           .speak("I am a text waiting for speech.", "appA");
    ASSERT_TRUE(speakResult) << "Error on speak";

    auto expectedValue = jsonEngine.get_value("TextToSpeech.speak");
    EXPECT_EQ(speakResult->speechid, expectedValue["speechid"].get<int32_t>());
    EXPECT_EQ(speakResult->TTS_status, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(speakResult->success, expectedValue["success"].get<bool>());
}

TEST_F(TextToSpeechTest, pause)
{
    int32_t speechId = 1;
    auto result = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().pause(speechId);
    ASSERT_TRUE(result) << "Error on pause";
}

TEST_F(TextToSpeechTest, resume)
{
    int32_t speechId = 1;
    auto result = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().resume(speechId);
    ASSERT_TRUE(result) << "Error on resume";
}

TEST_F(TextToSpeechTest, cancel)
{
    int32_t speechId = 1;
    auto result = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().cancel(speechId);
    ASSERT_TRUE(result) << "Error on cancel";
}

TEST_F(TextToSpeechTest, getSpeechState)
{
    int32_t speechId = 1;
    auto speechState = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().getspeechstate(speechId);
    ASSERT_TRUE(speechState) << "Error getting speech state";

    auto expectedValue = jsonEngine.get_value("TextToSpeech.getspeechstate");
    EXPECT_EQ(speechState->speechstate, std::to_string(expectedValue["speechstate"].get<int>()));
    EXPECT_EQ(speechState->TTS_status, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(speechState->success, expectedValue["success"].get<bool>());
}

TEST_F(TextToSpeechTest, subscribeOnWillSpeak)
{
    std::condition_variable cv;
    std::mutex mtx;
    bool eventReceived = false;

    auto id = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().subscribeOnWillspeak(
        [&](const auto& event)
        {
            EXPECT_EQ(event.speechid, 1);
            {
                std::lock_guard<std::mutex> lock(mtx);
                eventReceived = true;
            }
            cv.notify_one();
        });

    verifyEventSubscription(id);

    triggerEvent("TextToSpeech.onWillspeak", R"({ "speechid": 1 })");

    verifyEventReceived(mtx, cv, eventReceived);

    auto result = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().unsubscribe(id.value_or(0));
    verifyUnsubscribeResult(result);
}

TEST_F(TextToSpeechTest, subscribeOnSpeechStart)
{
    std::condition_variable cv;
    std::mutex mtx;
    bool eventReceived = false;

    auto id = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().subscribeOnSpeechstart(
        [&](const auto& event)
        {
            EXPECT_EQ(event.speechid, 1);
            EXPECT_EQ(event.text, std::nullopt);

            {
                std::lock_guard<std::mutex> lock(mtx);
                eventReceived = true;
            }
            cv.notify_one();
        });

    verifyEventSubscription(id);

    triggerEvent("TextToSpeech.onSpeechstart", R"({ "speechid": 1 })");

    verifyEventReceived(mtx, cv, eventReceived);

    auto result = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().unsubscribe(id.value_or(0));
    verifyUnsubscribeResult(result);
}

TEST_F(TextToSpeechTest, subscribeOnSpeechStartWithText)
{
    std::condition_variable cv;
    std::mutex mtx;
    bool eventReceived = false;

    auto id = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().subscribeOnSpeechstart(
        [&](const auto& event)
        {
            EXPECT_EQ(event.speechid, 1);
            EXPECT_EQ(event.text, "I am a text waiting for speech.");

            {
                std::lock_guard<std::mutex> lock(mtx);
                eventReceived = true;
            }
            cv.notify_one();
        });

    verifyEventSubscription(id);

    triggerEvent("TextToSpeech.onSpeechstart", R"({ "speechid": 1, "text": "I am a text waiting for speech." })");

    verifyEventReceived(mtx, cv, eventReceived);

    auto result = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().unsubscribe(id.value_or(0));
    verifyUnsubscribeResult(result);
}

TEST_F(TextToSpeechTest, subscribeOnSpeechComplete)
{
    std::condition_variable cv;
    std::mutex mtx;
    bool eventReceived = false;

    auto id = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().subscribeOnSpeechcomplete(
        [&](const auto& event)
        {
            EXPECT_EQ(event.speechid, 1);

            {
                std::lock_guard<std::mutex> lock(mtx);
                eventReceived = true;
            }
            cv.notify_one();
        });

    verifyEventSubscription(id);

    triggerEvent("TextToSpeech.onSpeechcomplete", R"({ "speechid": 1 })");

    verifyEventReceived(mtx, cv, eventReceived);

    auto result = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().unsubscribe(id.value_or(0));
    verifyUnsubscribeResult(result);
}

TEST_F(TextToSpeechTest, subscribeOnTtsStateChanged)
{
    std::condition_variable cv;
    std::mutex mtx;
    bool eventReceived = false;

    auto id = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().subscribeOnTTSstatechanged(
        [&](const auto& event)
        {
            EXPECT_EQ(event.state, 1);
            {
                std::lock_guard<std::mutex> lock(mtx);
                eventReceived = true;
            }
            cv.notify_one();
        });

    verifyEventSubscription(id);

    triggerEvent("TextToSpeech.onTTSstatechanged", R"({ "state": true })");

    verifyEventReceived(mtx, cv, eventReceived);

    auto result = Firebolt::IFireboltAccessor::Instance().TextToSpeechInterface().unsubscribe(id.value_or(0));
    verifyUnsubscribeResult(result);
}
