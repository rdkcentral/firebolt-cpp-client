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

#include "json_types/device.h"
#include "device_impl.h"
#include "json_engine.h"
#include "mock_helper.h"
#include <vector>

class DeviceUTest : public ::testing::Test, protected MockBase
{
protected:
    Firebolt::Device::DeviceImpl deviceImpl_{mockHelper};
};

TEST_F(DeviceUTest, ChipsetId)
{
    mock("Device.chipsetId");
    auto expectedValue = jsonEngine.get_value("Device.chipsetId");

    auto result = deviceImpl_.chipsetId();
    ASSERT_TRUE(result) << "DeviceImpl::chipsetId() returned an error";

    EXPECT_EQ(*result, expectedValue);
}

TEST_F(DeviceUTest, ChipsetIdBadResponse)
{
    mock_with_response("Device.chipsetId", 12345);
    auto result = deviceImpl_.chipsetId();
    ASSERT_FALSE(result) << "DeviceImpl::chipsetId() did not return an error";
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(DeviceUTest, DeviceClass)
{
    mock("Device.deviceClass");
    auto expectedValue = jsonEngine.get_value("Device.deviceClass");

    auto result = deviceImpl_.deviceClass();

    ASSERT_TRUE(result) << "DeviceImpl::deviceClass() returned an error";

    EXPECT_EQ(static_cast<int>(*result), static_cast<int>(Firebolt::Device::JsonData::DeviceClassEnum.at(expectedValue)));
}

TEST_F(DeviceUTest, OsName)
{
    mock_with_response("Device.osName", "Linux");

    auto result = deviceImpl_.osName();
    ASSERT_TRUE(result) << "DeviceImpl::osName() returned an error";
    EXPECT_EQ(*result, "Linux");
}

TEST_F(DeviceUTest, OsNameBadResponse)
{
    mock_with_response("Device.osName", 12345);
    auto result = deviceImpl_.osName();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(DeviceUTest, SetOsName)
{
    nlohmann::json expectedParams;
    expectedParams["value"] = "Linux";
    EXPECT_CALL(mockHelper, invoke("Device.setOsName", expectedParams))
        .WillOnce(Invoke([](const std::string&, const nlohmann::json&)
                         { return Firebolt::Result<void>{Firebolt::Error::None}; }));

    auto result = deviceImpl_.setOsName("Linux");

    ASSERT_TRUE(result);
}

TEST_F(DeviceUTest, OsVersion)
{

    mock_with_response("Device.osVersion", "5.15.0");

    auto result = deviceImpl_.osVersion();
    ASSERT_TRUE(result) << "DeviceImpl::osVersion() returned an error";
    EXPECT_EQ(*result, "5.15.0");
}

TEST_F(DeviceUTest, OsVersionBadResponse)
{
    mock_with_response("Device.osVersion", 12345);
    auto result = deviceImpl_.osVersion();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(DeviceUTest, SetOsVersion)
{
    nlohmann::json expectedParams;
    expectedParams["value"] = "5.15.0";
    EXPECT_CALL(mockHelper, invoke("Device.setOsVersion", expectedParams))
        .WillOnce(Invoke([](const std::string&, const nlohmann::json&)
                         { return Firebolt::Result<void>{Firebolt::Error::None}; }));

    auto result = deviceImpl_.setOsVersion("5.15.0");

    ASSERT_TRUE(result);
}

TEST_F(DeviceUTest, Firmware)
{
    mock_with_response("Device.firmware", "1.0.0-20240101");

    auto result = deviceImpl_.firmware();
    ASSERT_TRUE(result) << "DeviceImpl::firmware() returned an error";
    EXPECT_EQ(*result, "1.0.0-20240101");
}

TEST_F(DeviceUTest, FirmwareBadResponse)
{
    mock_with_response("Device.firmware", 12345);
    auto result = deviceImpl_.firmware();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(DeviceUTest, DeviceClassBadResponse)
{
    mock_with_response("Device.deviceClass", "abc");
    auto result = deviceImpl_.deviceClass();
    ASSERT_FALSE(result) << "DeviceImpl::deviceClass() didn't returned an error";
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(DeviceUTest, Hdr)
{
    mock("Device.hdr");
    auto expectedValue = jsonEngine.get_value("Device.hdr");

    auto result = deviceImpl_.hdr();

    ASSERT_TRUE(result) << "DeviceImpl::hdr() returned an error";

    EXPECT_EQ(result->hdr10, expectedValue["hdr10"].get<bool>());
    EXPECT_EQ(result->hdr10Plus, expectedValue["hdr10Plus"].get<bool>());
    EXPECT_EQ(result->dolbyVision, expectedValue["dolbyVision"].get<bool>());
    EXPECT_EQ(result->hlg, expectedValue["hlg"].get<bool>());
}

TEST_F(DeviceUTest, HdrBadResponse)
{
    mock_with_response("Device.hdr", nlohmann::json{{"hdr10", true}});
    auto result = deviceImpl_.hdr();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(DeviceUTest, TimeInActiveState)
{
    mock("Device.timeInActiveState");
    auto expectedValue = jsonEngine.get_value("Device.timeInActiveState");

    auto result = deviceImpl_.timeInActiveState();
    ASSERT_TRUE(result) << "DeviceImpl::timeInActiveState() returned an error";

    EXPECT_EQ(*result, expectedValue);
}

TEST_F(DeviceUTest, TimeInActiveStatePassesThroughAppStateInvalidError)
{
    constexpr auto appStateInvalid = static_cast<Firebolt::Error>(-50110);
    EXPECT_CALL(mockHelper, getJson("Device.timeInActiveState", _))
        .WillOnce(Invoke([&](const std::string& /*methodName*/, const nlohmann::json& /*parameters*/)
                         { return Firebolt::Result<nlohmann::json>{appStateInvalid}; }));

    auto result = deviceImpl_.timeInActiveState();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), appStateInvalid);
}

TEST_F(DeviceUTest, TimeInActiveStateBadResponse)
{
    mock_with_response("Device.timeInActiveState", "invalid_response");
    auto result = deviceImpl_.timeInActiveState();
    ASSERT_FALSE(result) << "DeviceImpl::timeInActiveState() did not return an error";
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(DeviceUTest, UnsignedGettersRejectInvalidNumbers)
{
    const std::vector<nlohmann::json> badUnsigned = {nlohmann::json(-1), nlohmann::json(4294967296LL),
                                                     nlohmann::json(1.5), nlohmann::json(true), nlohmann::json("5")};
    for (const char* method : {"Device.uptime", "Device.timeInActiveState"})
    {
        for (const auto& bad : badUnsigned)
        {
            mock_with_response(method, bad);
            auto result = std::string(method) == "Device.uptime" ? deviceImpl_.uptime() : deviceImpl_.timeInActiveState();
            ASSERT_FALSE(result) << method << " " << bad.dump();
            EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
        }
    }
}

TEST_F(DeviceUTest, UnsignedGettersAcceptTheFullRange)
{
    mock_with_response("Device.uptime", 4294967295LL);
    auto uptime = deviceImpl_.uptime();
    ASSERT_TRUE(uptime);
    EXPECT_EQ(*uptime, 4294967295U);

    mock_with_response("Device.timeInActiveState", 0);
    auto active = deviceImpl_.timeInActiveState();
    ASSERT_TRUE(active);
    EXPECT_EQ(*active, 0U);
}

TEST_F(DeviceUTest, Uid)
{
    mock("Device.uid");
    auto expectedValue = jsonEngine.get_value("Device.uid");

    auto result = deviceImpl_.uid();
    ASSERT_TRUE(result) << "DeviceImpl::uid() returned an error";

    EXPECT_EQ(*result, expectedValue);
}

TEST_F(DeviceUTest, UidBadResponse)
{
    mock_with_response("Device.uid", 67890);
    auto result = deviceImpl_.uid();
    ASSERT_FALSE(result) << "DeviceImpl::uid() did not return an error";
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}
TEST_F(DeviceUTest, Uptime)
{
    mock("Device.uptime");
    auto expectedValue = jsonEngine.get_value("Device.uptime");

    auto result = deviceImpl_.uptime();
    ASSERT_TRUE(result) << "DeviceImpl::uptime() returned an error";

    EXPECT_EQ(*result, expectedValue);
}

TEST_F(DeviceUTest, UptimeBadResponse)
{
    mock_with_response("Device.uptime", "invalid_response");
    auto result = deviceImpl_.uptime();
    ASSERT_FALSE(result) << "DeviceImpl::uptime() did not return an error";
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(DeviceUTest, SubscribeOnHdrChanged)
{
    nlohmann::json expectedValue = 1;
    mockSubscribe("Device.onHdrChanged");

    auto result = deviceImpl_.subscribeOnHdrChanged([&](const Firebolt::Device::HDRFormat& /*value*/) {});

    ASSERT_TRUE(result) << "DeviceImpl::subscribeOnHdrChanged() returned an error";
    EXPECT_EQ(*result, expectedValue);

    deviceImpl_.unsubscribe(*result);
}

TEST_F(DeviceUTest, DolbyAtmosExperienceAvailable)
{
    mock("Device.dolbyAtmosExperienceAvailable");
    auto expectedValue = jsonEngine.get_value("Device.dolbyAtmosExperienceAvailable");

    auto result = deviceImpl_.dolbyAtmosExperienceAvailable();
    ASSERT_TRUE(result) << "DeviceImpl::dolbyAtmosExperienceAvailable() returned an error";

    EXPECT_EQ(*result, expectedValue.get<bool>());
}

TEST_F(DeviceUTest, DolbyAtmosExperienceAvailableBadResponse)
{
    mock_with_response("Device.dolbyAtmosExperienceAvailable", "invalid_response");
    auto result = deviceImpl_.dolbyAtmosExperienceAvailable();
    ASSERT_FALSE(result) << "DeviceImpl::dolbyAtmosExperienceAvailable() did not return an error";
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(DeviceUTest, SubscribeOnDolbyAtmosExperienceAvailableChanged)
{
    nlohmann::json expectedValue = 1;
    mockSubscribe("Device.onDolbyAtmosExperienceAvailableChanged");

    auto result = deviceImpl_.subscribeOnDolbyAtmosExperienceAvailableChanged([&](const bool& /*value*/) {});

    ASSERT_TRUE(result) << "DeviceImpl::subscribeOnDolbyAtmosExperienceAvailableChanged() returned an error";
    EXPECT_EQ(*result, expectedValue);

    deviceImpl_.unsubscribe(*result);
}

TEST_F(DeviceUTest, Name)
{
    mock("Device.name");
    auto expectedValue = jsonEngine.get_value("Device.name");

    auto result = deviceImpl_.name();
    ASSERT_TRUE(result) << "DeviceImpl::name() returned an error";

    EXPECT_EQ(*result, expectedValue);
}

TEST_F(DeviceUTest, NameBadResponse)
{
    mock_with_response("Device.name", 12345);
    auto result = deviceImpl_.name();
    ASSERT_FALSE(result) << "DeviceImpl::name() did not return an error";
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(DeviceUTest, SubscribeOnNameChanged)
{
    nlohmann::json expectedValue = 1;
    mockSubscribe("Device.onNameChanged");

    auto result = deviceImpl_.subscribeOnNameChanged([&](const std::string& /*value*/) {});

    ASSERT_TRUE(result) << "DeviceImpl::subscribeOnNameChanged() returned an error";
    EXPECT_EQ(*result, expectedValue);

    deviceImpl_.unsubscribe(*result);
}
