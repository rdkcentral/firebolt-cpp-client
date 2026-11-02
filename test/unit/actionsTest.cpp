/**
 * Copyright 2025 Comcast Cable Communications Management, LLC
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

#include "actions_impl.h"
#include "json_engine.h"
#include "mock_helper.h"

using ::testing::Invoke;

class ActionsUTest : public ::testing::Test, protected MockBase
{
protected:
    Firebolt::Actions::ActionsImpl actionsImpl_{mockHelper};
};

TEST_F(ActionsUTest, Intent)
{
    constexpr auto intentJsonText =
        R"({"action":"pre-load","context":{"source":"system"},"data":{"ids":[1,"two",true,null],"future":{"enabled":false}}})";
    const auto expectedIntent = nlohmann::json::parse(intentJsonText);
    mock_with_response("Actions.intent", {{"intent", expectedIntent}, {"intentId", 0U}});

    auto result = actionsImpl_.intent();
    ASSERT_TRUE(result) << "ActionsImpl::intent() returned an error";
    EXPECT_EQ(result->intent, expectedIntent.dump());
    EXPECT_EQ(result->intentId, 0U);
}

TEST_F(ActionsUTest, IntentPreservesNonObjectPayload)
{
    const auto expectedIntent = nlohmann::json::array({"future-intent", 42, true, nullptr, {{"nested", {1, 2}}}});
    mock_with_response("Actions.intent", {{"intent", expectedIntent}, {"intentId", 1U}});

    auto result = actionsImpl_.intent();
    ASSERT_TRUE(result) << "ActionsImpl::intent() returned an error";
    EXPECT_EQ(result->intent, expectedIntent.dump());
}

TEST_F(ActionsUTest, SubscribeOnIntent)
{
    nlohmann::json expectedValue = 1;
    mockSubscribe("Actions.onIntent");

    auto result = actionsImpl_.subscribeOnIntent([&](const Firebolt::Actions::Intent& /*value*/) {});

    ASSERT_TRUE(result) << "ActionsImpl::subscribeOnIntent() returned an error";
    EXPECT_EQ(*result, expectedValue);

    auto unsubResult = actionsImpl_.unsubscribe(*result);
    ASSERT_TRUE(unsubResult) << "ActionsImpl::unsubscribe() returned an error";
}

TEST_F(ActionsUTest, Start)
{
    constexpr auto intentJsonText =
        R"({"action":"pre-load","data":{"ids":[1,"two",true,null],"future":{"enabled":false}}})";
    nlohmann::json expectedParams;
    expectedParams["intent"] = nlohmann::json::parse(intentJsonText);
    expectedParams["handlerAppId"] = "com.example.handler";
    EXPECT_CALL(mockHelper, invoke("Actions.start", expectedParams))
        .WillOnce(Invoke([&](const std::string& /*methodName*/, const nlohmann::json& /*parameters*/)
                         { return Firebolt::Result<void>{Firebolt::Error::None}; }));

    auto result = actionsImpl_.start(intentJsonText, "com.example.handler");
    ASSERT_TRUE(result) << "ActionsImpl::start() returned an error";
}

TEST_F(ActionsUTest, StartPreservesNonObjectPayload)
{
    const auto intentJson = nlohmann::json::array({"future-intent", 42, true, nullptr, {{"nested", {1, 2}}}});
    nlohmann::json expectedParams;
    expectedParams["intent"] = intentJson;
    EXPECT_CALL(mockHelper, invoke("Actions.start", expectedParams))
        .WillOnce(Invoke([&](const std::string& /*methodName*/, const nlohmann::json& /*parameters*/)
                         { return Firebolt::Result<void>{Firebolt::Error::None}; }));

    auto result = actionsImpl_.start(intentJson.dump());
    ASSERT_TRUE(result) << "ActionsImpl::start() returned an error";
}

TEST_F(ActionsUTest, StartRejectsMalformedJsonWithoutThrowing)
{
    auto result = actionsImpl_.start("{not valid json");
    ASSERT_FALSE(result) << "ActionsImpl::start() should reject malformed JSON";
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}
