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

#include "json_engine.h"
#include "mock_helper.h"
#include "texttospeech_impl.h"
#include "utils.h"
#include <list>

class TextToSpeechTest : public ::testing::Test, protected MockBase
{
protected:
    Firebolt::TextToSpeech::TextToSpeechImpl ttsImpl{mockHelper};
};

TEST_F(TextToSpeechTest, listVoices)
{
    mock("TextToSpeech.listvoices");

    auto voices = ttsImpl.listVoices("en-US");
    ASSERT_TRUE(voices);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.listvoices");
    EXPECT_EQ(voices->TTS_status, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(voices->voices.size(), expectedValue["voices"].size());
    for (size_t i = 0; i < voices->voices.size(); ++i)
    {
        EXPECT_EQ(voices->voices[i], expectedValue["voices"][i].get<std::string>());
    }
}

TEST_F(TextToSpeechTest, speak)
{
    mock("TextToSpeech.speak");

    auto speak = ttsImpl.speak("I am a text waiting for speech.");
    ASSERT_TRUE(speak);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.speak");
    EXPECT_EQ(speak->speechid, expectedValue["speechid"].get<int32_t>());
    EXPECT_EQ(speak->TTS_status, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(speak->success, expectedValue["success"].get<bool>());
}

TEST_F(TextToSpeechTest, pause)
{
    mock("TextToSpeech.pause");

    auto resp = ttsImpl.pause(1);
    ASSERT_TRUE(resp);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.pause");
    EXPECT_EQ(resp->TTS_status, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(resp->success, expectedValue["success"].get<bool>());
}

TEST_F(TextToSpeechTest, resume)
{
    mock("TextToSpeech.resume");

    auto resp = ttsImpl.resume(1);
    ASSERT_TRUE(resp);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.resume");
    EXPECT_EQ(resp->TTS_status, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(resp->success, expectedValue["success"].get<bool>());
}

TEST_F(TextToSpeechTest, cancel)
{
    mock("TextToSpeech.cancel");

    auto resp = ttsImpl.cancel(1);
    ASSERT_TRUE(resp);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.cancel");
    EXPECT_EQ(resp->TTS_status, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(resp->success, expectedValue["success"].get<bool>());
}

TEST_F(TextToSpeechTest, getSpeechState)
{
    mock("TextToSpeech.getspeechstate");

    auto speechStateResp = ttsImpl.getSpeechState(1);
    ASSERT_TRUE(speechStateResp);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.getspeechstate");

    EXPECT_EQ(speechStateResp->speechstate, std::to_string(expectedValue["speechstate"].get<int>()));
    EXPECT_EQ(speechStateResp->TTS_status, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(speechStateResp->success, expectedValue["success"].get<bool>());
}

TEST_F(TextToSpeechTest, subscribeOnWillSpeak)
{
    mockSubscribe("TextToSpeech.onWillspeak");

    auto id = ttsImpl.subscribeOnWillSpeak([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnSpeechStart)
{
    mockSubscribe("TextToSpeech.onSpeechstart");

    auto id = ttsImpl.subscribeOnSpeechStart([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnSpeechPause)
{
    mockSubscribe("TextToSpeech.onSpeechpause");

    auto id = ttsImpl.subscribeOnSpeechPause([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnSpeechResume)
{
    mockSubscribe("TextToSpeech.onSpeechresume");

    auto id = ttsImpl.subscribeOnSpeechResume([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnSpeechComplete)
{
    mockSubscribe("TextToSpeech.onSpeechcomplete");

    auto id = ttsImpl.subscribeOnSpeechComplete([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnSpeechInterrupted)
{
    mockSubscribe("TextToSpeech.onSpeechinterrupted");

    auto id = ttsImpl.subscribeOnSpeechInterrupted([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnNetworkError)
{
    mockSubscribe("TextToSpeech.onNetworkerror");

    auto id = ttsImpl.subscribeOnNetworkError([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnPlaybackError)
{
    mockSubscribe("TextToSpeech.onPlaybackerror");

    auto id = ttsImpl.subscribeOnPlaybackError([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}
