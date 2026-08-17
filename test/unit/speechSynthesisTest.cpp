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