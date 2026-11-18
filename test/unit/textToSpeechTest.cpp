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

#include "json_types/texttospeech.h"
#include "bcp47.h"
#include "firebolt/texttospeech.h"
#include "json_engine.h"
#include "mock_helper.h"
#include "texttospeech_impl.h"

class TextToSpeechUTest : public ::testing::Test, protected MockBase
{
protected:
    Firebolt::TextToSpeech::TextToSpeechImpl ttsImpl{mockHelper};
};

TEST_F(TextToSpeechUTest, listVoices)
{
    mock("TextToSpeech.listvoices");

    auto voices = ttsImpl.listVoices("en-US");
    ASSERT_TRUE(voices);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.listvoices");
    EXPECT_EQ(voices->ttsStatus, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(voices->voices.size(), expectedValue["voices"].size());
    for (size_t i = 0; i < voices->voices.size(); ++i)
    {
        EXPECT_EQ(voices->voices[i], expectedValue["voices"][i].get<std::string>());
    }
}

TEST_F(TextToSpeechUTest, listVoicesRejectsInvalidBcp47Language)
{
    EXPECT_CALL(mockHelper, getJson("TextToSpeech.listvoices", _)).Times(0);

    auto result = ttsImpl.listVoices("en--US");
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(TextToSpeechUTest, speakRejectsInvalidBcp47Language)
{
    EXPECT_CALL(mockHelper, getJson("TextToSpeech.speak", _)).Times(0);

    auto result = ttsImpl.speak("utterance", std::nullopt, std::string("en--US"));
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(TextToSpeechUTest, speakAcceptsBoundaryVolumeRateAndPitch)
{
    EXPECT_CALL(mockHelper, getJson("TextToSpeech.speak", _))
        .Times(2)
        .WillRepeatedly(Invoke([&](const std::string& /*methodName*/, const nlohmann::json& /*parameters*/)
                               { return Firebolt::Result<nlohmann::json>{jsonEngine.get_value("TextToSpeech.speak")}; }));

    EXPECT_TRUE(ttsImpl.speak("t", std::nullopt, std::nullopt, std::nullopt, std::string("0"), std::string("0.1"),
                              std::string("0")));
    EXPECT_TRUE(ttsImpl.speak("t", std::nullopt, std::nullopt, std::nullopt, std::string("1"), std::string("10"),
                              std::string("2")));
}

TEST_F(TextToSpeechUTest, speakRejectsInvalidVolumeRateOrPitch)
{
    EXPECT_CALL(mockHelper, getJson("TextToSpeech.speak", _)).Times(0);

    const std::optional<std::string> none;
    for (const char* bad : {"", "abc", "1.5", "-0.1", "nan", "inf", " 1", "1 ", "0,5", "+1", "-0", "1e0", ".", "0.5.1"})
    {
        auto volume = ttsImpl.speak("t", none, none, none, std::string(bad), none, none);
        ASSERT_FALSE(volume) << "volume " << bad;
        EXPECT_EQ(volume.error(), Firebolt::Error::InvalidParams);
    }
    for (const char* bad : {"", "0.09", "10.1", "fast", "-1"})
    {
        auto rate = ttsImpl.speak("t", none, none, none, none, std::string(bad), none);
        ASSERT_FALSE(rate) << "rate " << bad;
        EXPECT_EQ(rate.error(), Firebolt::Error::InvalidParams);
    }
    for (const char* bad : {"", "2.1", "-0.5", "high"})
    {
        auto pitch = ttsImpl.speak("t", none, none, none, none, none, std::string(bad));
        ASSERT_FALSE(pitch) << "pitch " << bad;
        EXPECT_EQ(pitch.error(), Firebolt::Error::InvalidParams);
    }
}

TEST(Bcp47Test, acceptsWellFormedTags)
{
    for (const char* tag : {"en", "en-US", "zh-Hant-TW", "es-419", "de-CH-1901", "en-US-u-ca-gregory", "x-private",
                            "i-klingon", "sgn-BE-FR", "EN-us"})
    {
        EXPECT_TRUE(Firebolt::Bcp47::isValid(tag)) << tag;
    }
}

TEST(Bcp47Test, rejectsMalformedTags)
{
    for (const char* tag : {"", "-", "en-", "-en", "en--US", "e", "en_US", "en-US-u-a-u-b", "en-x", "x", "en-US-u",
                            "123", "en-Latn-Latn-US", "toolonglang"})
    {
        EXPECT_FALSE(Firebolt::Bcp47::isValid(tag)) << tag;
    }
}

TEST_F(TextToSpeechUTest, listVoicesBadResponse)
{
    mock_with_response("TextToSpeech.listvoices",
                       nlohmann::json{{"TTS_Status", 0}, {"voices", "not-an-array-of-strings"}});
    auto result = ttsImpl.listVoices("en-US");
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(TextToSpeechUTest, pauseResumeCancelBadResponse)
{
    EXPECT_CALL(mockHelper, getJson(::testing::_, ::testing::_))
        .Times(3)
        .WillRepeatedly(Invoke([](const std::string&, const nlohmann::json&)
                               { return Firebolt::Result<nlohmann::json>{nlohmann::json{{"success", true}}}; }));

    const auto expectInvalidParams = [](const auto& result)
    {
        ASSERT_FALSE(result);
        EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
    };
    expectInvalidParams(ttsImpl.pause(1));
    expectInvalidParams(ttsImpl.resume(1));
    expectInvalidParams(ttsImpl.cancel(1));
}

TEST_F(TextToSpeechUTest, speak)
{
    mock("TextToSpeech.speak");

    auto speak = ttsImpl.speak("I am a text waiting for speech.");
    ASSERT_TRUE(speak);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.speak");
    EXPECT_EQ(speak->speechId, expectedValue["speechid"].get<int32_t>());
    EXPECT_EQ(speak->ttsStatus, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(speak->success, expectedValue["success"].get<bool>());
}

TEST_F(TextToSpeechUTest, speak_payloadDefaultsToTextOnly)
{
    EXPECT_CALL(mockHelper, getJson("TextToSpeech.speak", _))
        .WillOnce(Invoke(
            [&](const std::string& /*methodName*/, const nlohmann::json& parameters)
            {
                nlohmann::json expected = {{"text", "I am a text waiting for speech."}};
                EXPECT_EQ(parameters, expected) << "Parameters do not match expected payload: " << expected.dump()
                                                << " but got: " << parameters.dump();
                return Firebolt::Result<nlohmann::json>{jsonEngine.get_value("TextToSpeech.speak")};
            }));

    auto result = ttsImpl.speak("I am a text waiting for speech.");
    ASSERT_TRUE(result);
}

TEST_F(TextToSpeechUTest, speak_payloadIncludesAllOptionalFieldsWhenProvided)
{
    EXPECT_CALL(mockHelper, getJson("TextToSpeech.speak", _))
        .WillOnce(Invoke(
            [&](const std::string& /*methodName*/, const nlohmann::json& parameters)
            {
                nlohmann::json expected;
                expected["text"] = "I am a text waiting for speech.";
                expected["callsign"] = "AppA";
                expected["language"] = "en-US";
                expected["voice"] = "female-1";
                expected["volume"] = "0.8";
                expected["rate"] = "1";
                expected["pitch"] = "1";
                EXPECT_EQ(parameters, expected) << "Parameters do not match expected payload: " << expected.dump()
                                                << " but got: " << parameters.dump();
                return Firebolt::Result<nlohmann::json>{jsonEngine.get_value("TextToSpeech.speak")};
            }));

    auto result = ttsImpl.speak("I am a text waiting for speech.", std::string("AppA"), std::string("en-US"),
                                std::string("female-1"), std::string("0.8"), std::string("1"), std::string("1"));
    ASSERT_TRUE(result);
}

TEST_F(TextToSpeechUTest, speak_payloadIncludesOnlyProvidedOptionalFields)
{
    EXPECT_CALL(mockHelper, getJson("TextToSpeech.speak", _))
        .WillOnce(Invoke(
            [&](const std::string& /*methodName*/, const nlohmann::json& parameters)
            {
                nlohmann::json expected;
                expected["text"] = "I am a text waiting for speech.";
                expected["language"] = "en-US";
                expected["rate"] = "1";
                EXPECT_EQ(parameters, expected) << "Parameters do not match expected payload: " << expected.dump()
                                                << " but got: " << parameters.dump();
                return Firebolt::Result<nlohmann::json>{jsonEngine.get_value("TextToSpeech.speak")};
            }));

    auto result = ttsImpl.speak("I am a text waiting for speech.", std::nullopt, std::string("en-US"), std::nullopt,
                                std::nullopt, std::string("1"), std::nullopt);
    ASSERT_TRUE(result);
}

TEST_F(TextToSpeechUTest, speak_payloadOmitsUnsetOptionalKeys)
{
    EXPECT_CALL(mockHelper, getJson("TextToSpeech.speak", _))
        .WillOnce(Invoke(
            [&](const std::string& /*methodName*/, const nlohmann::json& parameters)
            {
                EXPECT_EQ(parameters.size(), 1U);
                EXPECT_TRUE(parameters.contains("text"));
                EXPECT_FALSE(parameters.contains("callsign"));
                EXPECT_FALSE(parameters.contains("language"));
                EXPECT_FALSE(parameters.contains("voice"));
                EXPECT_FALSE(parameters.contains("volume"));
                EXPECT_FALSE(parameters.contains("rate"));
                EXPECT_FALSE(parameters.contains("pitch"));
                return Firebolt::Result<nlohmann::json>{jsonEngine.get_value("TextToSpeech.speak")};
            }));

    auto result = ttsImpl.speak("I am a text waiting for speech.");
    ASSERT_TRUE(result);
}

TEST_F(TextToSpeechUTest, speak_payloadPreservesEmptyStringOptionalValues)
{
    EXPECT_CALL(mockHelper, getJson("TextToSpeech.speak", _))
        .WillOnce(Invoke(
            [&](const std::string& /*methodName*/, const nlohmann::json& parameters)
            {
                EXPECT_TRUE(parameters.contains("text"));
                EXPECT_TRUE(parameters.contains("callsign"));
                EXPECT_TRUE(parameters.contains("voice"));
                EXPECT_EQ(parameters["callsign"], "");
                EXPECT_EQ(parameters["voice"], "");
                EXPECT_FALSE(parameters.contains("language"));
                EXPECT_FALSE(parameters.contains("volume"));
                EXPECT_FALSE(parameters.contains("rate"));
                EXPECT_FALSE(parameters.contains("pitch"));
                return Firebolt::Result<nlohmann::json>{jsonEngine.get_value("TextToSpeech.speak")};
            }));

    auto result = ttsImpl.speak("I am a text waiting for speech.", std::string(""), std::nullopt, std::string(""),
                                std::nullopt, std::nullopt, std::nullopt);
    ASSERT_TRUE(result);
}

TEST_F(TextToSpeechUTest, speak_payloadUsesLowercaseCallsignKey)
{
    EXPECT_CALL(mockHelper, getJson("TextToSpeech.speak", _))
        .WillOnce(Invoke(
            [&](const std::string& /*methodName*/, const nlohmann::json& parameters)
            {
                EXPECT_TRUE(parameters.contains("callsign"));
                EXPECT_FALSE(parameters.contains("callSign"));
                EXPECT_EQ(parameters["callsign"], "AppA");
                return Firebolt::Result<nlohmann::json>{jsonEngine.get_value("TextToSpeech.speak")};
            }));

    auto result = ttsImpl.speak("I am a text waiting for speech.", std::string("AppA"));
    ASSERT_TRUE(result);
}

TEST_F(TextToSpeechUTest, speak_propagatesHelperError)
{
    EXPECT_CALL(mockHelper, getJson("TextToSpeech.speak", _))
        .WillOnce(Invoke([&](const std::string& /*methodName*/, const nlohmann::json& /*parameters*/)
                         { return Firebolt::Result<nlohmann::json>{Firebolt::Error::General}; }));

    auto result = ttsImpl.speak("I am a text waiting for speech.", std::string("AppA"));
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}

TEST_F(TextToSpeechUTest, pause)
{
    mock("TextToSpeech.pause");

    auto resp = ttsImpl.pause(1);
    ASSERT_TRUE(resp);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.pause");
    EXPECT_EQ(resp->ttsStatus, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(resp->success, expectedValue["success"].get<bool>());
}

TEST_F(TextToSpeechUTest, resume)
{
    mock("TextToSpeech.resume");

    auto resp = ttsImpl.resume(1);
    ASSERT_TRUE(resp);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.resume");
    EXPECT_EQ(resp->ttsStatus, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(resp->success, expectedValue["success"].get<bool>());
}

TEST_F(TextToSpeechUTest, cancel)
{
    mock("TextToSpeech.cancel");

    auto resp = ttsImpl.cancel(1);
    ASSERT_TRUE(resp);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.cancel");
    EXPECT_EQ(resp->ttsStatus, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(resp->success, expectedValue["success"].get<bool>());
}

TEST_F(TextToSpeechUTest, getSpeechState)
{
    mock("TextToSpeech.getspeechstate");

    auto speechStateResp = ttsImpl.getSpeechState(1);
    ASSERT_TRUE(speechStateResp);

    nlohmann::json expectedValue = jsonEngine.get_value("TextToSpeech.getspeechstate");

    EXPECT_EQ(speechStateResp->speechState,
              static_cast<Firebolt::TextToSpeech::SpeechState>(expectedValue["speechstate"].get<int>()));
    EXPECT_EQ(speechStateResp->ttsStatus, expectedValue["TTS_Status"].get<int32_t>());
    EXPECT_EQ(speechStateResp->success, expectedValue["success"].get<bool>());
}

TEST_F(TextToSpeechUTest, getSpeechStateMissingFieldReturnsInvalidParams)
{
    mock_with_response("TextToSpeech.getspeechstate", nlohmann::json{{"TTS_Status", 0}, {"success", true}});

    auto result = ttsImpl.getSpeechState(1);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(TextToSpeechUTest, getSpeechStateBadTypeReturnsInvalidParams)
{
    mock_with_response("TextToSpeech.getspeechstate",
                       nlohmann::json{{"speechstate", "in_progress"}, {"TTS_Status", 0}, {"success", true}});

    auto result = ttsImpl.getSpeechState(1);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(TextToSpeechUTest, getSpeechStateUnknownValueReturnsInvalidParams)
{
    mock_with_response("TextToSpeech.getspeechstate",
                       nlohmann::json{{"speechstate", 4}, {"TTS_Status", 0}, {"success", true}});

    auto result = ttsImpl.getSpeechState(1);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(TextToSpeechUTest, speakRejectsOutOfRangeTTSStatus)
{
    mock_with_response("TextToSpeech.speak", nlohmann::json{{"speechid", 1}, {"TTS_Status", 4}, {"success", true}});

    auto result = ttsImpl.speak("utterance");
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(TextToSpeechUTest, subscribeOnWillSpeak)
{
    mockSubscribe("TextToSpeech.onWillSpeak");

    auto id = ttsImpl.subscribeOnWillSpeak([](auto) {});
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechUTest, subscribeOnSpeechStart)
{
    mockSubscribe("TextToSpeech.onSpeechStart");

    auto id = ttsImpl.subscribeOnSpeechStart([](auto) {});
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechUTest, subscribeOnSpeechPause)
{
    mockSubscribe("TextToSpeech.onSpeechPause");

    auto id = ttsImpl.subscribeOnSpeechPause([](auto) {});
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechUTest, subscribeOnSpeechResume)
{
    mockSubscribe("TextToSpeech.onSpeechResume");

    auto id = ttsImpl.subscribeOnSpeechResume([](auto) {});
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechUTest, subscribeOnSpeechComplete)
{
    mockSubscribe("TextToSpeech.onSpeechComplete");

    auto id = ttsImpl.subscribeOnSpeechComplete([](auto) {});
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechUTest, subscribeOnSpeechInterrupted)
{
    mockSubscribe("TextToSpeech.onSpeechInterrupted");

    auto id = ttsImpl.subscribeOnSpeechInterrupted([](auto) {});
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechUTest, subscribeOnNetworkError)
{
    mockSubscribe("TextToSpeech.onNetworkError");

    auto id = ttsImpl.subscribeOnNetworkError([](auto) {});
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}

TEST_F(TextToSpeechUTest, subscribeOnPlaybackError)
{
    mockSubscribe("TextToSpeech.onPlaybackError");

    auto id = ttsImpl.subscribeOnPlaybackError([](auto) {});
    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_TRUE(id.has_value()) << "error on id";
    auto result = ttsImpl.unsubscribe(id.value_or(0));
    ASSERT_TRUE(result) << "error on unsubscribe ";
}
