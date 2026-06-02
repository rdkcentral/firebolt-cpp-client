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

#include "actions_impl.h"
#include "mock_helper.h"
#include <gtest/gtest.h>

class ActionsGeneratedUTest : public ::testing::Test
{
protected:
    ::testing::NiceMock<MockHelper> mockHelper;
    Firebolt::Actions::ActionsImpl impl{mockHelper};
};

TEST_F(ActionsGeneratedUTest, Constructs)
{
    SUCCEED();
}

TEST_F(ActionsGeneratedUTest, UnsubscribeForwardsToHelper)
{
    EXPECT_CALL(mockHelper, unsubscribe(7)).WillOnce(::testing::Return(Firebolt::Result<void>{Firebolt::Error::None}));

    auto result = impl.unsubscribe(7);
    ASSERT_TRUE(result) << "unsubscribe should return success when helper succeeds";
}

TEST_F(ActionsGeneratedUTest, ForwardsintentTransportErrors)
{
    EXPECT_CALL(mockHelper, getJson("Actions.intent", ::testing::_))
        .WillOnce(::testing::Invoke(
            [](const std::string& /*method*/, const nlohmann::json& params)
            {
                EXPECT_TRUE(params.is_null() || params.empty()) << "Expected no params for method call";
                return Firebolt::Result<nlohmann::json>{Firebolt::Error::General};
            }));

    auto result = impl.intent();
    EXPECT_FALSE(result) << "Expected error propagation when helper invoke fails";
}
