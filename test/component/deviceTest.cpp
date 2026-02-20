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
#include "json_types/jsondata_device_types.h"
#include "utils.h"
#include <gtest/gtest.h>
#include <iostream>

class DeviceTest : public ::testing::Test
{
protected:
    void SetUp() override { eventReceived = false; }

    std::condition_variable cv;
    std::mutex mtx;
    bool eventReceived;

    JsonEngine jsonEngine;
};

TEST_F(DeviceTest, ChipsetId)
{
    auto expectedValue = jsonEngine.get_value("Device.chipsetId");
    auto result = Firebolt::IFireboltAccessor::Instance().DeviceInterface().chipsetId();
    ASSERT_TRUE(result) << "DeviceImpl::chipsetId() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(DeviceTest, DeviceClass)
{
    auto expectedValue = jsonEngine.get_value("Device.deviceClass");
    auto result = Firebolt::IFireboltAccessor::Instance().DeviceInterface().deviceClass();
    ASSERT_TRUE(result) << "DeviceImpl::deviceClass() returned an error";
    EXPECT_EQ(static_cast<int>(*result), static_cast<int>(Firebolt::Device::JsonData::DeviceClassEnum.at(expectedValue)));
}

TEST_F(DeviceTest, Hdr)
{
    auto expectedValue = jsonEngine.get_value("Device.hdr");
    auto result = Firebolt::IFireboltAccessor::Instance().DeviceInterface().hdr();
    ASSERT_TRUE(result) << "DeviceImpl::hdr() returned an error";
    EXPECT_EQ(result->hdr10, expectedValue["hdr10"].get<bool>());
    EXPECT_EQ(result->hdr10Plus, expectedValue["hdr10Plus"].get<bool>());
    EXPECT_EQ(result->dolbyVision, expectedValue["dolbyVision"].get<bool>());
    EXPECT_EQ(result->hlg, expectedValue["hlg"].get<bool>());
}

TEST_F(DeviceTest, TimeInActiveState)
{
    auto expectedValue = jsonEngine.get_value("Device.timeInActiveState");
    auto result = Firebolt::IFireboltAccessor::Instance().DeviceInterface().timeInActiveState();
    ASSERT_TRUE(result) << "DeviceImpl::timeInActiveState() returned an error";
    if (expectedValue.empty())
    {
        std::cout << "[ !!!      ] Expected is empty, received: " << *result << std::endl;
        return;
    }
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(DeviceTest, Uid)
{
    auto expectedValue = jsonEngine.get_value("Device.uid");
    auto result = Firebolt::IFireboltAccessor::Instance().DeviceInterface().uid();
    ASSERT_TRUE(result) << "DeviceImpl::uid() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(DeviceTest, Uptime)
{
    auto expectedValue = jsonEngine.get_value("Device.uptime");
    auto result = Firebolt::IFireboltAccessor::Instance().DeviceInterface().uptime();
    ASSERT_TRUE(result) << "DeviceImpl::uptime() returned an error";
    if (expectedValue.empty())
    {
        std::cout << "[ !!!      ] Expected is empty, received: " << *result << std::endl;
        return;
    }
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(DeviceTest, SubscribeOnHdrChanged)
{
    auto id = Firebolt::IFireboltAccessor::Instance().DeviceInterface().subscribeOnHdrChanged(
        [&](const Firebolt::Device::HDRFormat& value)
        {
            std::cout << "[Subscription] Device HDR changed" << std::endl;
            EXPECT_EQ(value.hdr10, true);
            EXPECT_EQ(value.hdr10Plus, true);
            EXPECT_EQ(value.dolbyVision, true);
            EXPECT_EQ(value.hlg, true);
            {
                std::lock_guard<std::mutex> lock(mtx);
                eventReceived = true;
            }
            cv.notify_one();
        });

    verifyEventSubscription(id);

    triggerEvent("Device.onHdrChanged", R"({"hdr10": true, "hdr10Plus": true, "dolbyVision": true, "hlg": true})");
    verifyEventReceived(mtx, cv, eventReceived);

    auto result = Firebolt::IFireboltAccessor::Instance().DeviceInterface().unsubscribe(id.value());
    verifyUnsubscribeResult(result);
}
