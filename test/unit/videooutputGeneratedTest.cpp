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

#include "mock_helper.h"
#include "videooutput_impl.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

class VideooutputGeneratedUTest : public ::testing::Test
{
protected:
    static bool areGetterParamsEmpty(const nlohmann::json& params)
    {
        return params.is_null() || (params.is_object() && params.empty());
    }

    void expectGetterResponse(const std::string& methodName, const nlohmann::json& response)
    {
        EXPECT_CALL(mockHelper, getJson(methodName, ::testing::_))
            .WillOnce(::testing::Invoke([methodName, response](const std::string& /*method*/, const nlohmann::json& params)
                                        {
                                            EXPECT_TRUE(areGetterParamsEmpty(params))
                                                << methodName << " getter should not send request params";
                                            return Firebolt::Result<nlohmann::json>{response};
                                        }));
    }

    void expectGetterTransportError(const std::string& methodName, Firebolt::Error error = Firebolt::Error::General)
    {
        EXPECT_CALL(mockHelper, getJson(methodName, ::testing::_))
            .WillOnce(::testing::Invoke([methodName, error](const std::string& /*method*/, const nlohmann::json& params)
                                        {
                                            EXPECT_TRUE(areGetterParamsEmpty(params))
                                                << methodName << " getter should not send request params";
                                            return Firebolt::Result<nlohmann::json>{error};
                                        }));
    }

    ::testing::NiceMock<MockHelper> mockHelper;
    Firebolt::VideoOutput::VideoOutputImpl impl{mockHelper};
};

TEST_F(VideooutputGeneratedUTest, Constructs)
{
    SUCCEED();
}

TEST_F(VideooutputGeneratedUTest, UnsubscribeForwardsToHelper)
{
    EXPECT_CALL(mockHelper, unsubscribe(7)).WillOnce(::testing::Return(Firebolt::Result<void>{Firebolt::Error::None}));

    auto result = impl.unsubscribe(7);
    ASSERT_TRUE(result) << "unsubscribe should return success when helper succeeds";
}

TEST_F(VideooutputGeneratedUTest, UnsubscribeForwardsHelperErrors)
{
    EXPECT_CALL(mockHelper, unsubscribe(42))
        .WillOnce(::testing::Return(Firebolt::Result<void>{Firebolt::Error::General}));

    auto result = impl.unsubscribe(42);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}

TEST_F(VideooutputGeneratedUTest, UnsubscribeAllForwardsToHelper)
{
    EXPECT_CALL(mockHelper, unsubscribeAll(&impl)).Times(2);

    impl.unsubscribeAll();
}

TEST_F(VideooutputGeneratedUTest, ResolutionReturnsParsedValue)
{
    expectGetterResponse("VideoOutput.resolution", nlohmann::json{{"height", 1080}, {"width", 1920}});

    auto result = impl.resolution();
    ASSERT_TRUE(result);
    EXPECT_EQ(result->height, 1080U);
    EXPECT_EQ(result->width, 1920U);
}

TEST_F(VideooutputGeneratedUTest, ResolutionForwardsTransportErrors)
{
    expectGetterTransportError("VideoOutput.resolution");

    auto result = impl.resolution();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}

TEST_F(VideooutputGeneratedUTest, ResolutionReturnsInvalidParamsWhenPayloadIsMalformed)
{
    expectGetterResponse("VideoOutput.resolution", nlohmann::json{{"width", 1920}});

    auto result = impl.resolution();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(VideooutputGeneratedUTest, HdcpReturnsParsedValue)
{
    expectGetterResponse("VideoOutput.hdcp", nlohmann::json(static_cast<int>(Firebolt::VideoOutput::HdcpState::Hdcp22)));

    auto result = impl.hdcp();
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, Firebolt::VideoOutput::HdcpState::Hdcp22);
}

TEST_F(VideooutputGeneratedUTest, HdcpForwardsTransportErrors)
{
    expectGetterTransportError("VideoOutput.hdcp");

    auto result = impl.hdcp();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}

TEST_F(VideooutputGeneratedUTest, CecStateReturnsParsedValue)
{
    expectGetterResponse("VideoOutput.cecState",
                         nlohmann::json(static_cast<int>(Firebolt::VideoOutput::CecStateValue::Inactive)));

    auto result = impl.cecState();
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, Firebolt::VideoOutput::CecStateValue::Inactive);
}

TEST_F(VideooutputGeneratedUTest, CecStateForwardsTransportErrors)
{
    expectGetterTransportError("VideoOutput.cecState");

    auto result = impl.cecState();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}

TEST_F(VideooutputGeneratedUTest, RefreshRateReturnsParsedValue)
{
    expectGetterResponse("VideoOutput.refreshRate",
                         nlohmann::json(static_cast<int>(Firebolt::VideoOutput::RefreshRateValue::R5994)));

    auto result = impl.refreshRate();
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, Firebolt::VideoOutput::RefreshRateValue::R5994);
}

TEST_F(VideooutputGeneratedUTest, RefreshRateForwardsTransportErrors)
{
    expectGetterTransportError("VideoOutput.refreshRate");

    auto result = impl.refreshRate();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}

TEST_F(VideooutputGeneratedUTest, ColorDepthReturnsParsedValue)
{
    expectGetterResponse("VideoOutput.colorDepth",
                         nlohmann::json(static_cast<int>(Firebolt::VideoOutput::ColorDepthValue::D12)));

    auto result = impl.colorDepth();
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, Firebolt::VideoOutput::ColorDepthValue::D12);
}

TEST_F(VideooutputGeneratedUTest, ColorDepthForwardsTransportErrors)
{
    expectGetterTransportError("VideoOutput.colorDepth");

    auto result = impl.colorDepth();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}

TEST_F(VideooutputGeneratedUTest, ColorFormatReturnsParsedValue)
{
    expectGetterResponse("VideoOutput.colorFormat",
                         nlohmann::json(static_cast<int>(Firebolt::VideoOutput::ColorFormatValue::Ycbcr444)));

    auto result = impl.colorFormat();
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, Firebolt::VideoOutput::ColorFormatValue::Ycbcr444);
}

TEST_F(VideooutputGeneratedUTest, ColorFormatForwardsTransportErrors)
{
    expectGetterTransportError("VideoOutput.colorFormat");

    auto result = impl.colorFormat();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}

TEST_F(VideooutputGeneratedUTest, ColorimetryReturnsParsedValue)
{
    expectGetterResponse("VideoOutput.colorimetry",
                         nlohmann::json(static_cast<int>(Firebolt::VideoOutput::OutputColorimetry::Bt2020rgb)));

    auto result = impl.colorimetry();
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, Firebolt::VideoOutput::OutputColorimetry::Bt2020rgb);
}

TEST_F(VideooutputGeneratedUTest, ColorimetryForwardsTransportErrors)
{
    expectGetterTransportError("VideoOutput.colorimetry");

    auto result = impl.colorimetry();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}

TEST_F(VideooutputGeneratedUTest, DynamicRangeReturnsParsedValue)
{
    expectGetterResponse("VideoOutput.dynamicRange",
                         nlohmann::json(static_cast<int>(Firebolt::VideoOutput::DynamicRangeValue::Hdr10plus)));

    auto result = impl.dynamicRange();
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, Firebolt::VideoOutput::DynamicRangeValue::Hdr10plus);
}

TEST_F(VideooutputGeneratedUTest, DynamicRangeForwardsTransportErrors)
{
    expectGetterTransportError("VideoOutput.dynamicRange");

    auto result = impl.dynamicRange();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}

TEST_F(VideooutputGeneratedUTest, QuantizationRangeReturnsParsedValue)
{
    expectGetterResponse("VideoOutput.quantizationRange",
                         nlohmann::json(static_cast<int>(Firebolt::VideoOutput::QuantizationRangeValue::Limited)));

    auto result = impl.quantizationRange();
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, Firebolt::VideoOutput::QuantizationRangeValue::Limited);
}

TEST_F(VideooutputGeneratedUTest, QuantizationRangeForwardsTransportErrors)
{
    expectGetterTransportError("VideoOutput.quantizationRange");

    auto result = impl.quantizationRange();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}

TEST_F(VideooutputGeneratedUTest, SubscribeOnResolutionChangedForwardsAndDispatchesParsedPayload)
{
    bool notified = false;
    Firebolt::VideoOutput::VideoOutputResolution received{};

    EXPECT_CALL(mockHelper, subscribe(&impl, "VideoOutput.onResolutionChanged", ::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke([&](void* owner, const std::string& eventName, std::any&& notification,
                                        void (*callback)(void*, const nlohmann::json&))
                                    {
                                        Firebolt::Helpers::SubscriptionData data{owner, eventName,
                                                                                 std::move(notification)};
                                        callback(&data, nlohmann::json{{"height", 2160}, {"width", 3840}});
                                        return Firebolt::Result<Firebolt::SubscriptionId>{99};
                                    }));

    auto result = impl.subscribeOnResolutionChanged(
        [&](const Firebolt::VideoOutput::VideoOutputResolution& value)
        {
            notified = true;
            received = value;
        });

    ASSERT_TRUE(result);
    EXPECT_EQ(*result, 99U);
    EXPECT_TRUE(notified);
    EXPECT_EQ(received.height, 2160U);
    EXPECT_EQ(received.width, 3840U);
}

TEST_F(VideooutputGeneratedUTest, SubscribeOnResolutionChangedSwallowsMalformedEventPayload)
{
    bool notified = false;

    EXPECT_CALL(mockHelper, subscribe(&impl, "VideoOutput.onResolutionChanged", ::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke([&](void* owner, const std::string& eventName, std::any&& notification,
                                        void (*callback)(void*, const nlohmann::json&))
                                    {
                                        Firebolt::Helpers::SubscriptionData data{owner, eventName,
                                                                                 std::move(notification)};
                                        callback(&data, nlohmann::json{{"width", 3840}});
                                        return Firebolt::Result<Firebolt::SubscriptionId>{5};
                                    }));

    auto result = impl.subscribeOnResolutionChanged(
        [&](const Firebolt::VideoOutput::VideoOutputResolution& /*value*/) { notified = true; });

    ASSERT_TRUE(result);
    EXPECT_EQ(*result, 5U);
    EXPECT_FALSE(notified);
}

TEST_F(VideooutputGeneratedUTest, SubscribeOnResolutionChangedForwardsSubscribeErrors)
{
    EXPECT_CALL(mockHelper, subscribe(&impl, "VideoOutput.onResolutionChanged", ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(Firebolt::Result<Firebolt::SubscriptionId>{Firebolt::Error::General}));

    auto result = impl.subscribeOnResolutionChanged(
        [](const Firebolt::VideoOutput::VideoOutputResolution& /*value*/) {});

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}

TEST_F(VideooutputGeneratedUTest, SubscribeOnHdcpChangedForwardsAndDispatchesParsedPayload)
{
    bool notified = false;
    Firebolt::VideoOutput::HdcpState received = Firebolt::VideoOutput::HdcpState::None;

    EXPECT_CALL(mockHelper, subscribe(&impl, "VideoOutput.onHdcpChanged", ::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke([&](void* owner, const std::string& eventName, std::any&& notification,
                                        void (*callback)(void*, const nlohmann::json&))
                                    {
                                        Firebolt::Helpers::SubscriptionData data{owner, eventName,
                                                                                 std::move(notification)};
                                        callback(&data,
                                                 nlohmann::json(static_cast<int>(Firebolt::VideoOutput::HdcpState::Direct)));
                                        return Firebolt::Result<Firebolt::SubscriptionId>{11};
                                    }));

    auto result = impl.subscribeOnHdcpChanged(
        [&](const Firebolt::VideoOutput::HdcpState& value)
        {
            notified = true;
            received = value;
        });

    ASSERT_TRUE(result);
    EXPECT_EQ(*result, 11U);
    EXPECT_TRUE(notified);
    EXPECT_EQ(received, Firebolt::VideoOutput::HdcpState::Direct);
}

TEST_F(VideooutputGeneratedUTest, SubscribeOnHdcpChangedForwardsSubscribeErrors)
{
    EXPECT_CALL(mockHelper, subscribe(&impl, "VideoOutput.onHdcpChanged", ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(Firebolt::Result<Firebolt::SubscriptionId>{Firebolt::Error::General}));

    auto result = impl.subscribeOnHdcpChanged([](const Firebolt::VideoOutput::HdcpState& /*value*/) {});

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}

TEST_F(VideooutputGeneratedUTest, SubscribeOnCecStateChangedForwardsAndDispatchesParsedPayload)
{
    bool notified = false;
    Firebolt::VideoOutput::CecStateValue received = Firebolt::VideoOutput::CecStateValue::Unsupported;

    EXPECT_CALL(mockHelper, subscribe(&impl, "VideoOutput.onCecStateChanged", ::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke([&](void* owner, const std::string& eventName, std::any&& notification,
                                        void (*callback)(void*, const nlohmann::json&))
                                    {
                                        Firebolt::Helpers::SubscriptionData data{owner, eventName,
                                                                                 std::move(notification)};
                                        callback(&data,
                                                 nlohmann::json(static_cast<int>(Firebolt::VideoOutput::CecStateValue::Active)));
                                        return Firebolt::Result<Firebolt::SubscriptionId>{12};
                                    }));

    auto result = impl.subscribeOnCecStateChanged(
        [&](const Firebolt::VideoOutput::CecStateValue& value)
        {
            notified = true;
            received = value;
        });

    ASSERT_TRUE(result);
    EXPECT_EQ(*result, 12U);
    EXPECT_TRUE(notified);
    EXPECT_EQ(received, Firebolt::VideoOutput::CecStateValue::Active);
}

TEST_F(VideooutputGeneratedUTest, SubscribeOnCecStateChangedForwardsSubscribeErrors)
{
    EXPECT_CALL(mockHelper, subscribe(&impl, "VideoOutput.onCecStateChanged", ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(Firebolt::Result<Firebolt::SubscriptionId>{Firebolt::Error::General}));

    auto result = impl.subscribeOnCecStateChanged([](const Firebolt::VideoOutput::CecStateValue& /*value*/) {});

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}

TEST_F(VideooutputGeneratedUTest, SubscribeOnRefreshRateChangedForwardsAndDispatchesParsedPayload)
{
    bool notified = false;
    Firebolt::VideoOutput::RefreshRateValue received = Firebolt::VideoOutput::RefreshRateValue::R0;

    EXPECT_CALL(mockHelper, subscribe(&impl, "VideoOutput.onRefreshRateChanged", ::testing::_, ::testing::_))
        .WillOnce(::testing::Invoke([&](void* owner, const std::string& eventName, std::any&& notification,
                                        void (*callback)(void*, const nlohmann::json&))
                                    {
                                        Firebolt::Helpers::SubscriptionData data{owner, eventName,
                                                                                 std::move(notification)};
                                        callback(&data,
                                                 nlohmann::json(static_cast<int>(Firebolt::VideoOutput::RefreshRateValue::R24)));
                                        return Firebolt::Result<Firebolt::SubscriptionId>{13};
                                    }));

    auto result = impl.subscribeOnRefreshRateChanged(
        [&](const Firebolt::VideoOutput::RefreshRateValue& value)
        {
            notified = true;
            received = value;
        });

    ASSERT_TRUE(result);
    EXPECT_EQ(*result, 13U);
    EXPECT_TRUE(notified);
    EXPECT_EQ(received, Firebolt::VideoOutput::RefreshRateValue::R24);
}

TEST_F(VideooutputGeneratedUTest, SubscribeOnRefreshRateChangedForwardsSubscribeErrors)
{
    EXPECT_CALL(mockHelper, subscribe(&impl, "VideoOutput.onRefreshRateChanged", ::testing::_, ::testing::_))
        .WillOnce(::testing::Return(Firebolt::Result<Firebolt::SubscriptionId>{Firebolt::Error::General}));

    auto result = impl.subscribeOnRefreshRateChanged(
        [](const Firebolt::VideoOutput::RefreshRateValue& /*value*/) {});

    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}
