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

#include "json_engine.h"
#include "mock_helper.h"
#include "network_impl.h"

class NetworkUTest : public ::testing::Test, protected MockBase
{
protected:
    Firebolt::Network::NetworkImpl networkImpl_{mockHelper};
};

TEST_F(NetworkUTest, Connected)
{
    mock("Network.connected");
    auto expectedValue = jsonEngine.get_value("Network.connected");

    auto result = networkImpl_.connected();
    ASSERT_TRUE(result) << "NetworkImpl::connected() returned an error";

    EXPECT_EQ(*result, expectedValue.get<bool>());
}

TEST_F(NetworkUTest, SubscribeOnConnectedChanged)
{
    nlohmann::json expectedValue = 1;
    mockSubscribe("Network.onConnectedChanged");

    auto result = networkImpl_.subscribeOnConnectedChanged([&](const bool& /*value*/) {});

    ASSERT_TRUE(result) << "NetworkImpl::subscribeOnConnectedChanged() returned an error";
    EXPECT_EQ(*result, expectedValue);

    networkImpl_.unsubscribe(*result);
}
