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

#include "mock_helper.h"
#include "speechsynthesis_impl.h"

class SpeechSynthesisUTest : public ::testing::Test, protected MockBase
{
protected:
    Firebolt::SpeechSynthesis::SpeechSynthesisImpl speechSynthesisImpl_{mockHelper};
};

TEST_F(SpeechSynthesisUTest, Constructs)
{
    SUCCEED();
}

TEST_F(SpeechSynthesisUTest, speak)
{
    EXPECT_CALL(mockHelper, getJson("SpeechSynthesis.speak", _))
        .WillOnce(Invoke([&](const std::string& /*methodName*/, const nlohmann::json& /*parameters*/)
                         { return Firebolt::Result<nlohmann::json>{42}; }));

    auto result = speechSynthesisImpl_.speak("Hello from speech synthesis");

    ASSERT_TRUE(result);
    EXPECT_EQ(*result, 42U);
}

TEST_F(SpeechSynthesisUTest, speak_payloadIncludesOnlyProvidedOptionalFields)
{
    EXPECT_CALL(mockHelper, getJson("SpeechSynthesis.speak", _))
        .WillOnce(Invoke(
            [&](const std::string& /*methodName*/, const nlohmann::json& parameters)
            {
                nlohmann::json expected;
                expected["text"] = "payload";
                expected["language"] = "en-US";
                expected["pitch"] = "medium";
                EXPECT_EQ(parameters, expected);
                return Firebolt::Result<nlohmann::json>{7};
            }));

    auto result = speechSynthesisImpl_.speak("payload", std::nullopt, std::string("en-US"), std::nullopt, std::nullopt,
                                             std::nullopt, std::string("medium"));

    ASSERT_TRUE(result);
    EXPECT_EQ(*result, 7U);
}

TEST_F(SpeechSynthesisUTest, speak_invalidResponse)
{
    mock_with_response("SpeechSynthesis.speak", "not-a-number");

    auto result = speechSynthesisImpl_.speak("Hello from speech synthesis");

    ASSERT_FALSE(result);
}

TEST_F(SpeechSynthesisUTest, voices)
{
    nlohmann::json response = nlohmann::json::array();
    response.push_back({{"name", "Salli"}, {"lang", "en-US"}, {"default", true}});
    response.push_back({{"name", "Arthur"}, {"lang", "en-GB"}, {"default", false}});
    mock_with_response("SpeechSynthesis.voices", response);

    auto result = speechSynthesisImpl_.voices();

    ASSERT_TRUE(result);
    ASSERT_EQ(result->size(), 2U);
    EXPECT_EQ((*result)[0].name, "Salli");
    EXPECT_EQ((*result)[0].lang, "en-US");
    EXPECT_TRUE((*result)[0]._default);
    EXPECT_EQ((*result)[1].name, "Arthur");
    EXPECT_EQ((*result)[1].lang, "en-GB");
    EXPECT_FALSE((*result)[1]._default);
}

TEST_F(SpeechSynthesisUTest, voices_payloadHasNoParameters)
{
    EXPECT_CALL(mockHelper, getJson("SpeechSynthesis.voices", _))
        .WillOnce(Invoke(
            [&](const std::string& /*methodName*/, const nlohmann::json& parameters)
            {
                EXPECT_TRUE(parameters.is_object());
                EXPECT_TRUE(parameters.empty());
                nlohmann::json response = nlohmann::json::array();
                response.push_back({{"name", "Salli"}, {"lang", "en-US"}, {"default", true}});
                return Firebolt::Result<nlohmann::json>{response};
            }));

    auto result = speechSynthesisImpl_.voices();

    ASSERT_TRUE(result);
    ASSERT_EQ(result->size(), 1U);
}

TEST_F(SpeechSynthesisUTest, voices_invalidResponse)
{
    nlohmann::json badResponse = {{"name", "not-an-array"}};
    mock_with_response("SpeechSynthesis.voices", badResponse);

    auto result = speechSynthesisImpl_.voices();

    ASSERT_FALSE(result);
}

TEST_F(SpeechSynthesisUTest, subscribeOnVoicesChanged)
{
    EXPECT_CALL(mockHelper, subscribe(_, "SpeechSynthesis.onVoicesChanged", _, _))
        .WillOnce(::testing::Return(Firebolt::Result<Firebolt::SubscriptionId>{1}));

    auto result = speechSynthesisImpl_.subscribeOnVoicesChanged(
        [](const std::pmr::vector<Firebolt::SpeechSynthesis::Voice>& /*voices*/) {});
    ASSERT_TRUE(result) << "error on subscribe ";
    EXPECT_TRUE(result.has_value()) << "error on id";
}

TEST_F(SpeechSynthesisUTest, subscribeOnVoicesChanged_subscribeError)
{
    EXPECT_CALL(mockHelper, subscribe(_, "SpeechSynthesis.onVoicesChanged", _, _))
        .WillOnce(::testing::Return(Firebolt::Result<Firebolt::SubscriptionId>{Firebolt::Error::General}));

    auto result = speechSynthesisImpl_.subscribeOnVoicesChanged(
        [](const std::pmr::vector<Firebolt::SpeechSynthesis::Voice>& /*voices*/) {});
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}

TEST_F(SpeechSynthesisUTest, cancel)
{
    nlohmann::json expectedParams;
    expectedParams["id"] = static_cast<Firebolt::SpeechSynthesis::UtteranceId>(123);

    EXPECT_CALL(mockHelper, invoke("SpeechSynthesis.cancel", expectedParams))
        .WillOnce(::testing::Return(Firebolt::Result<void>{Firebolt::Error::None}));

    auto result = speechSynthesisImpl_.cancel(static_cast<Firebolt::SpeechSynthesis::UtteranceId>(123));
    ASSERT_TRUE(result);
}

TEST_F(SpeechSynthesisUTest, cancel_invokeError)
{
    EXPECT_CALL(mockHelper, invoke("SpeechSynthesis.cancel", _))
        .WillOnce(::testing::Return(Firebolt::Result<void>{Firebolt::Error::General}));

    auto result = speechSynthesisImpl_.cancel(static_cast<Firebolt::SpeechSynthesis::UtteranceId>(123));
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}