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

TEST(VideooutputGeneratedCTest, MarshallersRejectUnknownWireValues)
{
    Firebolt::VideoOutput::JsonData::HdcpStateJson hdcpJson;
    Firebolt::VideoOutput::JsonData::CecStateValueJson cecStateJson;
    Firebolt::VideoOutput::JsonData::ColorFormatValueJson colorFormatJson;
    Firebolt::VideoOutput::JsonData::DynamicRangeValueJson dynamicRangeJson;
    Firebolt::VideoOutput::JsonData::QuantizationRangeValueJson quantizationRangeJson;
    Firebolt::VideoOutput::JsonData::RefreshRateValueJson refreshRateJson;

    EXPECT_THROW(hdcpJson.fromJson(nlohmann::json("hdcp3.0")), std::out_of_range);
    EXPECT_THROW(cecStateJson.fromJson(nlohmann::json("not-a-state")), std::out_of_range);
    EXPECT_THROW(colorFormatJson.fromJson(nlohmann::json("xyz")), std::out_of_range);
    EXPECT_THROW(dynamicRangeJson.fromJson(nlohmann::json("hdr11")), std::out_of_range);
    EXPECT_THROW(quantizationRangeJson.fromJson(nlohmann::json("super")), std::out_of_range);
    EXPECT_THROW(refreshRateJson.fromJson(nlohmann::json("61")), std::out_of_range);
}

TEST(VideooutputGeneratedCTest, InterfaceSurfaceHasresolution)
{
    using Interface = Firebolt::VideoOutput::IVideoOutput;
    auto ptr = &Interface::resolution;
    (void)ptr;
    SUCCEED();
}

TEST(VideooutputGeneratedCTest, InterfaceSurfaceHascolorDepth)
{
    using Interface = Firebolt::VideoOutput::IVideoOutput;
    auto ptr = &Interface::colorDepth;
    (void)ptr;
    SUCCEED();
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

TEST_F(VideooutputGeneratedRuntimeCTest, SubscribeOnRefreshRateChangedParsesWireStringPayload)
{
    auto id = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().subscribeOnRefreshRateChanged(
        [&](const Firebolt::VideoOutput::RefreshRateValue& value)
        {
            EXPECT_EQ(value, Firebolt::VideoOutput::RefreshRateValue::R5994);
            {
                std::lock_guard<std::mutex> lock(mtx);
                eventReceived = true;
            }
            cv.notify_one();
        });

    if (!id)
    {
        GTEST_SKIP() << "VideoOutput.onRefreshRateChanged is not available in the current mock OpenRPC fixture";
    }
    verifyEventSubscription(id);
    triggerEvent("VideoOutput.onRefreshRateChanged", R"("59.94")");
    verifyEventReceived(mtx, cv, eventReceived);

    resetEventState();
    triggerEvent("VideoOutput.onRefreshRateChanged", R"("61")");
    verifyEventNotReceived(mtx, cv, eventReceived);

    auto result = Firebolt::IFireboltAccessor::Instance().VideoOutputInterface().unsubscribe(id.value());
    verifyUnsubscribeResult(result);
}
