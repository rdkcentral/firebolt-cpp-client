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

#include "firebolt/firebolt.h"
#include "utils.h"
#include <gtest/gtest.h>

class SpeechSynthesisCTest : public ::testing::Test
{
protected:
    std::condition_variable cv_;
    std::mutex mtx_;
    bool eventReceived_ = false;
};

TEST_F(SpeechSynthesisCTest, speak)
{
    auto result = Firebolt::IFireboltAccessor::Instance().SpeechSynthesisInterface().speak("component test");
    ASSERT_TRUE(result) << "SpeechSynthesis.speak unavailable";
    EXPECT_GT(*result, 0U);
}

TEST_F(SpeechSynthesisCTest, voices)
{
    auto result = Firebolt::IFireboltAccessor::Instance().SpeechSynthesisInterface().voices();
    ASSERT_TRUE(result) << "SpeechSynthesis.voices unavailable";
    EXPECT_GE(result->size(), 1U);
    EXPECT_FALSE((*result)[0].name.empty());
    EXPECT_FALSE((*result)[0].lang.empty());
}

TEST_F(SpeechSynthesisCTest, cancelPauseResume)
{
    constexpr Firebolt::SpeechSynthesis::UtteranceId id = 1;

    auto cancelResult = Firebolt::IFireboltAccessor::Instance().SpeechSynthesisInterface().cancel(id);
    ASSERT_TRUE(cancelResult) << "SpeechSynthesis.cancel unavailable";

    auto pauseResult = Firebolt::IFireboltAccessor::Instance().SpeechSynthesisInterface().pause(id);
    ASSERT_TRUE(pauseResult) << "pause failed after successful cancel call";

    auto resumeResult = Firebolt::IFireboltAccessor::Instance().SpeechSynthesisInterface().resume(id);
    ASSERT_TRUE(resumeResult) << "resume failed after successful cancel call";
}

TEST_F(SpeechSynthesisCTest, subscribeOnVoicesChanged)
{
    auto id = Firebolt::IFireboltAccessor::Instance().SpeechSynthesisInterface().subscribeOnVoicesChanged(
        [&](const std::pmr::vector<Firebolt::SpeechSynthesis::Voice>& voices)
        {
            ASSERT_EQ(voices.size(), 1U);
            EXPECT_EQ(voices[0].name, "Salli");
            EXPECT_EQ(voices[0].lang, "en-US");
            EXPECT_TRUE(voices[0]._default);
            {
                std::lock_guard<std::mutex> lock(mtx_);
                eventReceived_ = true;
            }
            cv_.notify_one();
        });

    verifyEventSubscription(id);

    triggerEvent("SpeechSynthesis.onVoicesChanged", R"([{"name":"Salli","lang":"en-US","default":true}])");
    verifyEventReceived(mtx_, cv_, eventReceived_);
}

TEST_F(SpeechSynthesisCTest, subscribeOnUtteranceEvent)
{
    auto id = Firebolt::IFireboltAccessor::Instance().SpeechSynthesisInterface().subscribeOnUtteranceEvent(
        [&](const Firebolt::SpeechSynthesis::UtteranceEvent& event)
        {
            EXPECT_EQ(event.id, static_cast<Firebolt::SpeechSynthesis::UtteranceId>(7));
            EXPECT_EQ(event.event, Firebolt::SpeechSynthesis::UtteranceEventEnum::resumed);
            {
                std::lock_guard<std::mutex> lock(mtx_);
                eventReceived_ = true;
            }
            cv_.notify_one();
        });

    verifyEventSubscription(id);

    triggerEvent("SpeechSynthesis.onUtteranceEvent", R"({"id":7,"event":"resumed"})");
    verifyEventReceived(mtx_, cv_, eventReceived_);
}
