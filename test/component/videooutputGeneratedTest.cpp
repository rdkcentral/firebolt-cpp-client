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
#include "firebolt/videooutput.h"
#include "json_engine.h"
#include "json_types/videooutput.h"
#include "utils.h"
#include <condition_variable>
#include <gtest/gtest.h>
#include <mutex>

class VideooutputGeneratedRuntimeCTest : public ::testing::Test
{
protected:
    void SetUp() override { eventReceived = false; }

    void resetEventState()
    {
        std::lock_guard<std::mutex> lock(mtx);
        eventReceived = false;
    }

    std::condition_variable cv;
    std::mutex mtx;
    bool eventReceived{false};
    JsonEngine jsonEngine;
};

TEST(VideooutputGeneratedCTest, HdcpMarshallerParsesWireString)
{
    Firebolt::VideoOutput::JsonData::HdcpStateJson jsonType;
    jsonType.fromJson(nlohmann::json("hdcp1.4"));

    EXPECT_EQ(jsonType.value(), Firebolt::VideoOutput::HdcpState::Hdcp14);
}

TEST(VideooutputGeneratedCTest, CecStateMarshallerParsesWireString)
{
    Firebolt::VideoOutput::JsonData::CecStateValueJson jsonType;
    jsonType.fromJson(nlohmann::json("inactive"));

    EXPECT_EQ(jsonType.value(), Firebolt::VideoOutput::CecStateValue::Inactive);
}

TEST(VideooutputGeneratedCTest, ColorFormatMarshallerParsesWireString)
{
    Firebolt::VideoOutput::JsonData::ColorFormatValueJson jsonType;
    jsonType.fromJson(nlohmann::json("ycbcr422"));

    EXPECT_EQ(jsonType.value(), Firebolt::VideoOutput::ColorFormatValue::Ycbcr422);
}

TEST(VideooutputGeneratedCTest, DynamicRangeMarshallerParsesWireString)
{
    Firebolt::VideoOutput::JsonData::DynamicRangeValueJson jsonType;
    jsonType.fromJson(nlohmann::json("sdr"));

    EXPECT_EQ(jsonType.value(), Firebolt::VideoOutput::DynamicRangeValue::Sdr);
}

TEST(VideooutputGeneratedCTest, QuantizationRangeMarshallerParsesWireString)
{
    Firebolt::VideoOutput::JsonData::QuantizationRangeValueJson jsonType;
    jsonType.fromJson(nlohmann::json("limited"));

    EXPECT_EQ(jsonType.value(), Firebolt::VideoOutput::QuantizationRangeValue::Limited);
}

TEST(VideooutputGeneratedCTest, RefreshRateMarshallerParsesWireString)
{
    Firebolt::VideoOutput::JsonData::RefreshRateValueJson jsonType;
    jsonType.fromJson(nlohmann::json("59.94"));

    EXPECT_EQ(jsonType.value(), Firebolt::VideoOutput::RefreshRateValue::R5994);
}

TEST(VideooutputGeneratedCTest, RefreshRateMarshallerParsesWholeNumberFloat)
{
    // Some servers encode a whole-number refresh rate (e.g. 24) as a JSON float literal (24.0).
    Firebolt::VideoOutput::JsonData::RefreshRateValueJson jsonType;
    jsonType.fromJson(nlohmann::json::parse("24.0"));

    EXPECT_EQ(jsonType.value(), Firebolt::VideoOutput::RefreshRateValue::R24);
}

TEST(VideooutputGeneratedCTest, MarshallersRejectUnknownWireValues)
{
    Firebolt::VideoOutput::JsonData::HdcpStateJson hdcpJson;
    Firebolt::VideoOutput::JsonData::CecStateValueJson cecStateJson;
    Firebolt::VideoOutput::JsonData::ColorFormatValueJson colorFormatJson;
    Firebolt::VideoOutput::JsonData::DynamicRangeValueJson dynamicRangeJson;
    Firebolt::VideoOutput::JsonData::QuantizationRangeValueJson quantizationRangeJson;
    Firebolt::VideoOutput::JsonData::RefreshRateValueJson refreshRateJson;
    Firebolt::VideoOutput::JsonData::ColorDepthValueJson colorDepthJson;

    EXPECT_THROW(hdcpJson.fromJson(nlohmann::json("hdcp3.0")), std::out_of_range);
    EXPECT_THROW(cecStateJson.fromJson(nlohmann::json("not-a-state")), std::out_of_range);
    EXPECT_THROW(colorFormatJson.fromJson(nlohmann::json("xyz")), std::out_of_range);
    EXPECT_THROW(dynamicRangeJson.fromJson(nlohmann::json("hdr11")), std::out_of_range);
    EXPECT_THROW(quantizationRangeJson.fromJson(nlohmann::json("super")), std::out_of_range);
    EXPECT_THROW(refreshRateJson.fromJson(nlohmann::json("61")), std::out_of_range);
    EXPECT_THROW(refreshRateJson.fromJson(nlohmann::json(59)), std::out_of_range);
    EXPECT_THROW(colorDepthJson.fromJson(nlohmann::json("11")), std::out_of_range);
}

TEST_F(VideooutputGeneratedRuntimeCTest, ResolutionReturnsParsedValue)
{
    const auto expected = jsonEngine.get_value("VideoOutput.resolution");
    const auto result = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().resolution();

    ASSERT_TRUE(result) << "VideoOutputImpl::resolution() returned an error";
    EXPECT_EQ(result->height, expected.at("height").get<uint32_t>());
    EXPECT_EQ(result->width, expected.at("width").get<uint32_t>());
}

TEST_F(VideooutputGeneratedRuntimeCTest, HdcpReturnsParsedValue)
{
    const auto expected = jsonEngine.get_value("VideoOutput.hdcp");
    const auto result = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().hdcp();

    ASSERT_TRUE(result) << "VideoOutputImpl::hdcp() returned an error";
    EXPECT_EQ(*result, Firebolt::VideoOutput::JsonData::HdcpStateEnum.at(expected.get<std::string>()));
}

TEST_F(VideooutputGeneratedRuntimeCTest, CecStateReturnsParsedValue)
{
    const auto expected = jsonEngine.get_value("VideoOutput.cecState");
    const auto result = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().cecState();

    ASSERT_TRUE(result) << "VideoOutputImpl::cecState() returned an error";
    EXPECT_EQ(*result, Firebolt::VideoOutput::JsonData::CecStateValueEnum.at(expected.get<std::string>()));
}

TEST_F(VideooutputGeneratedRuntimeCTest, RefreshRateReturnsParsedNumericValue)
{
    const auto expected = jsonEngine.get_value("VideoOutput.refreshRate");
    const auto result = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().refreshRate();

    ASSERT_TRUE(result) << "VideoOutputImpl::refreshRate() returned an error";
    EXPECT_DOUBLE_EQ(expected.get<double>(), 59.94);
    EXPECT_EQ(*result, Firebolt::VideoOutput::RefreshRateValue::R5994);
}

TEST_F(VideooutputGeneratedRuntimeCTest, ColorDepthReturnsParsedNumericValue)
{
    const auto expected = jsonEngine.get_value("VideoOutput.colorDepth");
    const auto result = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().colorDepth();

    ASSERT_TRUE(result) << "VideoOutputImpl::colorDepth() returned an error";
    EXPECT_EQ(expected.get<int>(), 8);
    EXPECT_EQ(*result, Firebolt::VideoOutput::ColorDepthValue::D8);
}

TEST_F(VideooutputGeneratedRuntimeCTest, ColorFormatReturnsParsedValue)
{
    const auto expected = jsonEngine.get_value("VideoOutput.colorFormat");
    const auto result = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().colorFormat();

    ASSERT_TRUE(result) << "VideoOutputImpl::colorFormat() returned an error";
    EXPECT_EQ(*result, Firebolt::VideoOutput::JsonData::ColorFormatValueEnum.at(expected.get<std::string>()));
}

TEST_F(VideooutputGeneratedRuntimeCTest, ColorimetryReturnsParsedValue)
{
    const auto expected = jsonEngine.get_value("VideoOutput.colorimetry");
    const auto result = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().colorimetry();

    ASSERT_TRUE(result) << "VideoOutputImpl::colorimetry() returned an error";
    EXPECT_EQ(*result, Firebolt::VideoOutput::JsonData::OutputColorimetryEnum.at(expected.get<std::string>()));
}

TEST_F(VideooutputGeneratedRuntimeCTest, DynamicRangeReturnsParsedValue)
{
    const auto expected = jsonEngine.get_value("VideoOutput.dynamicRange");
    const auto result = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().dynamicRange();

    ASSERT_TRUE(result) << "VideoOutputImpl::dynamicRange() returned an error";
    EXPECT_EQ(*result, Firebolt::VideoOutput::JsonData::DynamicRangeValueEnum.at(expected.get<std::string>()));
}

TEST_F(VideooutputGeneratedRuntimeCTest, QuantizationRangeReturnsParsedValue)
{
    const auto expected = jsonEngine.get_value("VideoOutput.quantizationRange");
    const auto result = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().quantizationRange();

    ASSERT_TRUE(result) << "VideoOutputImpl::quantizationRange() returned an error";
    EXPECT_EQ(*result, Firebolt::VideoOutput::JsonData::QuantizationRangeValueEnum.at(expected.get<std::string>()));
}

TEST_F(VideooutputGeneratedRuntimeCTest, SubscribeOnHdcpChangedParsesWireStringPayload)
{
    auto id = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().subscribeOnHdcpChanged(
        [&](const Firebolt::VideoOutput::HdcpState& value)
        {
            EXPECT_EQ(value, Firebolt::VideoOutput::HdcpState::Hdcp14);
            {
                std::lock_guard<std::mutex> lock(mtx);
                eventReceived = true;
            }
            cv.notify_one();
        });

    verifyEventSubscription(id);
    triggerEvent("VideoOutput.onHdcpChanged", R"("hdcp1.4")");
    verifyEventReceived(mtx, cv, eventReceived);

    resetEventState();
    triggerEvent("VideoOutput.onHdcpChanged", R"("invalid-hdcp")");
    verifyEventNotReceived(mtx, cv, eventReceived);

    auto result = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().unsubscribe(id.value());
    verifyUnsubscribeResult(result);
}

TEST_F(VideooutputGeneratedRuntimeCTest, SubscribeOnRefreshRateChangedParsesWireNumericPayload)
{
    auto id = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().subscribeOnRefreshRateChanged(
        [&](const Firebolt::VideoOutput::RefreshRateValue& value)
        {
            EXPECT_EQ(value, Firebolt::VideoOutput::RefreshRateValue::R24);
            {
                std::lock_guard<std::mutex> lock(mtx);
                eventReceived = true;
            }
            cv.notify_one();
        });

    verifyEventSubscription(id);
    // Whole-number rates can be encoded as a float on the wire (e.g. 24.0); the marshaller must still
    // resolve it to R24 instead of rejecting it.
    triggerEvent("VideoOutput.onRefreshRateChanged", "24.0");
    verifyEventReceived(mtx, cv, eventReceived);

    resetEventState();
    triggerEvent("VideoOutput.onRefreshRateChanged", "61.0");
    verifyEventNotReceived(mtx, cv, eventReceived);

    auto result = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().unsubscribe(id.value());
    verifyUnsubscribeResult(result);
}

TEST_F(VideooutputGeneratedRuntimeCTest, SubscribeOnCecStateChangedParsesWireStringPayload)
{
    auto id = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().subscribeOnCecStateChanged(
        [&](const Firebolt::VideoOutput::CecStateValue& value)
        {
            EXPECT_EQ(value, Firebolt::VideoOutput::CecStateValue::Inactive);
            {
                std::lock_guard<std::mutex> lock(mtx);
                eventReceived = true;
            }
            cv.notify_one();
        });

    verifyEventSubscription(id);
    triggerEvent("VideoOutput.onCecStateChanged", R"("inactive")");
    verifyEventReceived(mtx, cv, eventReceived);

    resetEventState();
    triggerEvent("VideoOutput.onCecStateChanged", R"("invalid-cec")");
    verifyEventNotReceived(mtx, cv, eventReceived);

    auto result = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().unsubscribe(id.value());
    verifyUnsubscribeResult(result);
}
