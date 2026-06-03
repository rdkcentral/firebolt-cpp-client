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

class ActionsUTest : public ::testing::Test, protected MockBase
{
protected:
    Firebolt::Actions::ActionsImpl actionsImpl_{mockHelper};
};

TEST_F(ActionsUTest, Start)
{
    mock_with_response("Actions.intent", "launch");

    auto result = actionsImpl_.intent();
    ASSERT_TRUE(result) << "ActionsImpl::intent() returned an error";
    EXPECT_EQ(*result, "launch");
}

TEST_F(ActionsUTest, SubscribeOnIntent)
{
    nlohmann::json expectedValue = 1;
    mockSubscribe("Actions.onIntent");

    auto result = actionsImpl_.subscribeOnIntent([&](const std::string& /*value*/) {});

    ASSERT_TRUE(result) << "ActionsImpl::subscribeOnIntent() returned an error";
    EXPECT_EQ(*result, expectedValue);

    auto unsubResult = actionsImpl_.unsubscribe(*result);
    ASSERT_TRUE(unsubResult) << "ActionsImpl::unsubscribe() returned an error";
}
