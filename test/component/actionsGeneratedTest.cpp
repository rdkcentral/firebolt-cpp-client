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
#include <condition_variable>
#include <gtest/gtest.h>
#include <mutex>

class ActionsGeneratedCTest : public ::testing::Test
{
protected:
    void SetUp() override { eventReceived = false; }

    std::condition_variable cv;
    std::mutex mtx;
    bool eventReceived;
};

TEST_F(ActionsGeneratedCTest, Intent)
{
    auto result = Firebolt::IFireboltAccessor::Instance().ActionsInterface().intent();
    ASSERT_TRUE(result) << toError(result);
    auto parsed = nlohmann::json::parse(*result);
    EXPECT_TRUE(parsed.at("intent").is_object());
    EXPECT_EQ(parsed.at("intent").at("action").get<std::string>(), "pre-load");
    EXPECT_EQ(parsed.at("intent").at("context").at("source").get<std::string>(), "system");
    EXPECT_EQ(parsed.at("intentId").get<unsigned>(), 0u);
}

TEST_F(ActionsGeneratedCTest, SubscribeOnIntent)
{
    auto id = Firebolt::IFireboltAccessor::Instance().ActionsInterface().subscribeOnIntent(
        [&](const std::string& intent)
        {
            auto parsed = nlohmann::json::parse(intent);
            EXPECT_TRUE(parsed.at("intent").is_object());
            EXPECT_EQ(parsed.at("intent").at("action").get<std::string>(), "pre-load");
            EXPECT_EQ(parsed.at("intent").at("context").at("source").get<std::string>(), "system");
            EXPECT_EQ(parsed.at("intentId").get<unsigned>(), 0u);
            {
                std::lock_guard<std::mutex> lock(mtx);
                eventReceived = true;
            }
            cv.notify_one();
        });

    ASSERT_TRUE(id) << toError(id);
    verifyEventSubscription(id);

    triggerEvent("Actions.onIntent", R"({"intent":{"action":"pre-load","context":{"source":"system"}},"intentId":0})");
    verifyEventReceived(mtx, cv, eventReceived);

    auto result = Firebolt::IFireboltAccessor::Instance().ActionsInterface().unsubscribe(id.value());
    verifyUnsubscribeResult(result);
}
