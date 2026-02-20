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

TEST_F(TextToSpeechTest, isTtsEnabled)
{
    mock("TextToSpeech.isttsenabled");

    auto ttsEnabled = ttsImpl.isttsenabled();
    ASSERT_TRUE(ttsEnabled);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.isttsenabled");
    EXPECT_EQ(ttsEnabled->TTS_status, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(ttsEnabled->isenabled, expectedValue["isenabled"].get<bool>());
}

TEST_F(TextToSpeechTest, listVoices)
{
    mock("TextToSpeech.listvoices");

    auto voices = ttsImpl.listvoices("en-US");
    ASSERT_TRUE(voices);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.listvoices");
    EXPECT_EQ(voices->TTS_status, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(voices->voices.size(), expectedValue["voices"].size());
    for (size_t i = 0; i < voices->voices.size(); ++i)
    {
        EXPECT_EQ(voices->voices[i], expectedValue["voices"][i].get<std::string>());
    }
}

TEST_F(TextToSpeechTest, getTtsConfiguration)
{
    mock("TextToSpeech.getttsconfiguration");

    auto ttsConfiguration = ttsImpl.getttsconfiguration();
    ASSERT_TRUE(ttsConfiguration);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.getttsconfiguration");
    EXPECT_TRUE(*ttsConfiguration == expectedValue);
}

TEST_F(TextToSpeechTest, getTtsConfigurationCustom)
{
    std::list<nlohmann::json> configs = {
        nlohmann::json{{"success", true},
                       {"ttsendpoint", "http://custom-tts-server.com"},
                       {"ttsendpointsecured", "https://custom-tts-server.com"},
                       {"language", "de-DE"},
                       {"voice", "anna"},
                       {"volume", 75},
                       {"primvolduckpercent", 30},
                       {"rate", 60},
                       {"speechrate", "fast"},
                       {"fallbacktext", {{"scenario", "custom_scenario"}, {"value", "Custom fallback message"}}}},
        nlohmann::json{{"success", true},
                       {"ttsendpoint", "http://custom-tts-server.com"},
                       {"ttsendpointsecured", "https://custom-tts-server.com"},
                       {"language", "de-DE"},
                       {"rate", 60},
                       {"speechrate", "fast"},
                       {"fallbacktext",
                        {
                            {"scenario", "custom_scenario"},
                        }}},
        nlohmann::json{{"success", true}, {"fallbacktext", {{"value", "Custom fallback message"}}}},
    };

    for (const auto& config : configs)
    {
        mock_with_response("TextToSpeech.getttsconfiguration", config);

        auto ttsConfiguration = ttsImpl.getttsconfiguration();
        ASSERT_TRUE(ttsConfiguration);

        EXPECT_TRUE(*ttsConfiguration == config);
    }
}

TEST_F(TextToSpeechTest, speak)
{
    mock("TextToSpeech.speak");

    auto speak = ttsImpl.speak("I am a text waiting for speech.", "appA");
    ASSERT_TRUE(speak);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.speak");
    EXPECT_EQ(speak->speechid, expectedValue["speechid"].get<int32_t>());
    EXPECT_EQ(speak->TTS_status, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(speak->success, expectedValue["success"].get<bool>());
}

TEST_F(TextToSpeechTest, setTtsConfiguration)
{
    mock("TextToSpeech.setttsconfiguration");

    auto resp =
        ttsImpl.setttsconfiguration("http://url_for_the_text_to_speech_processing_unit",
                                    "https://url_for_the_text_to_speech_processing_unit", "en-US", "carol", 100, 25, 50,
                                    Firebolt::TextToSpeech::SpeechRate::MEDIUM,
                                    Firebolt::TextToSpeech::FallbackText{"offline", "No Internet connection"});
    ASSERT_TRUE(resp);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.setttsconfiguration");
    EXPECT_EQ(resp->TTS_status, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(resp->success, expectedValue["success"].get<bool>());
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

    auto speechStateResp = ttsImpl.getspeechstate(1);
    ASSERT_TRUE(speechStateResp);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.getspeechstate");

    EXPECT_EQ(speechStateResp->speechstate, std::to_string(expectedValue["speechstate"].get<int>()));
    EXPECT_EQ(speechStateResp->TTS_status, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(speechStateResp->success, expectedValue["success"].get<bool>());
}

TEST_F(TextToSpeechTest, subscribeOnWillSpeak)
{
    mockSubscribe("texttospeech.onWillspeak");

    auto id = ttsImpl.subscribeOnWillspeak([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnSpeechStart)
{
    mockSubscribe("texttospeech.onSpeechstart");

    auto id = ttsImpl.subscribeOnSpeechstart([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnSpeechPause)
{
    mockSubscribe("texttospeech.onSpeechpause");

    auto id = ttsImpl.subscribeOnSpeechpause([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnSpeechResume)
{
    mockSubscribe("texttospeech.onSpeechresume");

    auto id = ttsImpl.subscribeOnSpeechresume([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnSpeechComplete)
{
    mockSubscribe("texttospeech.onSpeechcomplete");

    auto id = ttsImpl.subscribeOnSpeechcomplete([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnSpeechInterrupted)
{
    mockSubscribe("texttospeech.onSpeechinterrupted");

    auto id = ttsImpl.subscribeOnSpeechinterrupted([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnNetworkError)
{
    mockSubscribe("texttospeech.onNetworkerror");

    auto id = ttsImpl.subscribeOnNetworkerror([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnPlaybackError)
{
    mockSubscribe("texttospeech.onPlaybackError");

    auto id = ttsImpl.subscribeOnPlaybackerror([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnTtsStateChanged)
{
    mockSubscribe("texttospeech.onTTSstatechanged");

    auto id = ttsImpl.subscribeOnTTSstatechanged([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechTest, subscribeOnVoiceChanged)
{
    mockSubscribe("texttospeech.onVoicechanged");

    auto id = ttsImpl.subscribeOnVoicechanged([](auto) { std::cout << "callback\n"; });
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}
