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

using ::testing::_;
using ::testing::Invoke;

class ActionsUTest : public ::testing::Test, protected MockBase
{
protected:
    Firebolt::Actions::ActionsImpl actionsImpl_{mockHelper};
};

TEST_F(ActionsUTest, Intent)
{
    mock_with_response("Actions.intent",
                       nlohmann::json({{"intent", {{"action", "pre-load"}, {"context", {{"source", "system"}}}}},
                                       {"intentId", 0u}}));

    auto result = actionsImpl_.intent();
    ASSERT_TRUE(result) << "ActionsImpl::intent() returned an error";
    EXPECT_EQ(result->intent.action, "pre-load");
    EXPECT_EQ(result->intent.context.source, "system");
    EXPECT_EQ(result->intentId, 0u);
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
    nlohmann::json expectedParams;
    expectedParams["intent"] = {{"action", "pre-load"}, {"context", {{"source", "system"}}}};
    EXPECT_CALL(mockHelper, invoke("Actions.start", expectedParams))
        .WillOnce(Invoke([&](const std::string& /*methodName*/, const nlohmann::json& /*parameters*/)
                         { return Firebolt::Result<void>{Firebolt::Error::None}; }));

    auto result = actionsImpl_.start(R"({"action":"pre-load","context":{"source":"system"}})");
    ASSERT_TRUE(result) << "ActionsImpl::start() returned an error";
}

TEST_F(ActionsUTest, StartInvalidJson)
{
    auto result = actionsImpl_.start("not-valid-json");
    ASSERT_FALSE(result) << "ActionsImpl::start() should fail for invalid JSON";
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(ActionsUTest, StartNonObjectJson)
{
    auto result = actionsImpl_.start(R"([1,2,3])");
    ASSERT_FALSE(result) << "ActionsImpl::start() should fail for non-object JSON";
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}
