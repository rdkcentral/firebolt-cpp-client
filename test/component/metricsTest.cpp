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

class MetricsCTest : public ::testing::Test
{
protected:
    void SetUp() override {}

    JsonEngine jsonEngine;
};

TEST_F(MetricsCTest, Ready)
{
    auto expectedValue = jsonEngine.get_value("Metrics.ready");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().ready();
    ASSERT_TRUE(result) << "MetricsImpl::ready() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, SignIn)
{
    auto expectedValue = jsonEngine.get_value("Metrics.signIn");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().signIn();
    ASSERT_TRUE(result) << "MetricsImpl::signIn() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, SignOut)
{
    auto expectedValue = jsonEngine.get_value("Metrics.signOut");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().signOut();
    ASSERT_TRUE(result) << "MetricsImpl::signOut() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, StartContent)
{
    auto expectedValue = jsonEngine.get_value("Metrics.startContent");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().startContent("entity123",
                                                                                          Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::startContent() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, StopContent)
{
    auto expectedValue = jsonEngine.get_value("Metrics.stopContent");
    auto result =
        Firebolt::IFireboltAccessor::Instance().MetricsInterface().stopContent("entity123", Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::stopContent() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, Page)
{
    auto expectedValue = jsonEngine.get_value("Metrics.page");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().page("homePage", Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::page() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, Error)
{
    auto expectedValue = jsonEngine.get_value("Metrics.error");
    auto result = Firebolt::IFireboltAccessor::Instance()
                      .MetricsInterface()
                      .error(Firebolt::Metrics::ErrorType::Network, "ERR001", "Network error occurred", true,
                             std::map<std::string, std::string>{{"param1", "value1"}, {"param2", "value2"}},
                             Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::error() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, ErrorNoParameters)
{
    auto expectedValue = jsonEngine.get_value("Metrics.error");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().error(Firebolt::Metrics::ErrorType::Network,
                                                                                   "ERR001", "Network error occurred",
                                                                                   true, std::nullopt,
                                                                                   Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::error() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, ErrorNoAgePolicy)
{
    auto expectedValue = jsonEngine.get_value("Metrics.error");
    auto result = Firebolt::IFireboltAccessor::Instance()
                      .MetricsInterface()
                      .error(Firebolt::Metrics::ErrorType::Network, "ERR001", "Network error occurred", true,
                             std::map<std::string, std::string>{{"param1", "value1"}, {"param2", "value2"}},
                             std::nullopt);
    ASSERT_TRUE(result) << "MetricsImpl::error() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, MediaLoadStart)
{
    auto expectedValue = jsonEngine.get_value("Metrics.mediaLoadStart");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaLoadStart("mediaEntity123",
                                                                                            Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaLoadStart() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, MediaPlay)
{
    auto expectedValue = jsonEngine.get_value("Metrics.mediaPlay");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaPlay("mediaEntity123",
                                                                                       Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaPlay() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, MediaPlaying)
{
    auto expectedValue = jsonEngine.get_value("Metrics.mediaPlaying");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaPlaying("mediaEntity123",
                                                                                          Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaPlaying() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, MediaPause)
{
    auto expectedValue = jsonEngine.get_value("Metrics.mediaPause");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaPause("mediaEntity123",
                                                                                        Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaPause() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, MediaWaiting)
{
    auto expectedValue = jsonEngine.get_value("Metrics.mediaWaiting");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaWaiting("mediaEntity123",
                                                                                          Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaWaiting() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, MediaSeeking)
{
    auto expectedValue = jsonEngine.get_value("Metrics.mediaSeeking");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaSeeking("mediaEntity123", 0.5,
                                                                                          Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaSeeking() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, MediaSeekingInt)
{
    auto expectedValue = jsonEngine.get_value("Metrics.mediaSeeking");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaSeeking("mediaEntity123", 500,
                                                                                          Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaSeeking() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, MediaSeeked)
{
    auto expectedValue = jsonEngine.get_value("Metrics.mediaSeeked");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaSeeked("mediaEntity123", 0.5,
                                                                                         Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaSeeked() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, MediaSeekedInt)
{
    auto expectedValue = jsonEngine.get_value("Metrics.mediaSeeked");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaSeeked("mediaEntity123", 500,
                                                                                         Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaSeeked() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, MediaRateChanged)
{
    auto expectedValue = jsonEngine.get_value("Metrics.mediaRateChanged");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaRateChanged("mediaEntity123", 1.5,
                                                                                              Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaRateChanged() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, MediaRenditionChanged)
{
    auto expectedValue = jsonEngine.get_value("Metrics.mediaRenditionChanged");
    auto result =
        Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaRenditionChanged("mediaEntity123", 3000, 1920,
                                                                                         1080, "HDR",
                                                                                         Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaRenditionChanged() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, MediaRenditionChangedNoProfile)
{
    auto expectedValue = jsonEngine.get_value("Metrics.mediaRenditionChanged");
    auto result =
        Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaRenditionChanged("mediaEntity123", 3000, 1920,
                                                                                         1080, std::nullopt,
                                                                                         Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaRenditionChanged() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, MediaRenditionChangedNoAgePolicy)
{
    auto expectedValue = jsonEngine.get_value("Metrics.mediaRenditionChanged");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaRenditionChanged("mediaEntity123",
                                                                                                   3000, 1920, 1080,
                                                                                                   std::nullopt,
                                                                                                   std::nullopt);
    ASSERT_TRUE(result) << "MetricsImpl::mediaRenditionChanged() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, MediaEnded)
{
    auto expectedValue = jsonEngine.get_value("Metrics.mediaEnded");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaEnded("mediaEntity123",
                                                                                        Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaEnded() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, Event)
{
    auto expectedValue = jsonEngine.get_value("Metrics.event");
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().event("https://com.example.event",
                                                                                   "{\"key\":\"value\"}",
                                                                                   Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::event() returned an error";
    EXPECT_EQ(*result, expectedValue);
}

TEST_F(MetricsCTest, AppInfo)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().appInfo("1.0.0");
    ASSERT_TRUE(result) << "MetricsImpl::appInfo() returned an error";
}
