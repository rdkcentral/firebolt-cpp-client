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

#include "firebolt/firebolt.h"
#include "json_engine.h"
#include "utils.h"
#include <gtest/gtest.h>
#include <iostream>

class NetworkTest : public ::testing::Test
{
protected:
    void SetUp() override { eventReceived = false; }

    std::condition_variable cv;
    std::mutex mtx;
    bool eventReceived;

    JsonEngine jsonEngine;
};

TEST_F(NetworkTest, Connected)
{
    auto expectedValue = jsonEngine.get_value("Network.connected");
    auto result = Firebolt::IFireboltAccessor::Instance().NetworkInterface().connected();
    ASSERT_TRUE(result) << "NetworkImpl::connected() returned an error";
    EXPECT_EQ(*result, expectedValue.get<bool>());
}

TEST_F(NetworkTest, SubscribeOnConnectedChanged)
{
    auto id = Firebolt::IFireboltAccessor::Instance().NetworkInterface().subscribeOnConnectedChanged(
        [&](const bool& value)
        {
            std::cout << "[Subscription] Network connected changed" << std::endl;
            EXPECT_EQ(value, true);
            {
                std::lock_guard<std::mutex> lock(mtx);
                eventReceived = true;
            }
            cv.notify_one();
        });

    verifyEventSubscription(id);
    triggerEvent("Network.onConnectedChanged", R"({ "value": true })");

    verifyEventReceived(mtx, cv, eventReceived);

    auto result = Firebolt::IFireboltAccessor::Instance().NetworkInterface().unsubscribe(id.value());
    verifyUnsubscribeResult(result);
}
