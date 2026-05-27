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
#include "utils.h"
#include <gtest/gtest.h>

class MetricsCTest : public ::testing::Test
{
protected:
    void SetUp() override {}
};

TEST_F(MetricsCTest, Ready)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().ready();
    ASSERT_TRUE(result) << "MetricsImpl::ready() returned an error";
}

TEST_F(MetricsCTest, SignIn)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().signIn();
    ASSERT_TRUE(result) << "MetricsImpl::signIn() returned an error";
}

TEST_F(MetricsCTest, SignOut)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().signOut();
    ASSERT_TRUE(result) << "MetricsImpl::signOut() returned an error";
}

TEST_F(MetricsCTest, StartContent)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().startContent("entity123",
                                                                                          Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::startContent() returned an error";
}

TEST_F(MetricsCTest, StopContent)
{
    auto result =
        Firebolt::IFireboltAccessor::Instance().MetricsInterface().stopContent("entity123", Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::stopContent() returned an error";
}

TEST_F(MetricsCTest, Page)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().page("homePage", Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::page() returned an error";
}

TEST_F(MetricsCTest, Error)
{
    auto result = Firebolt::IFireboltAccessor::Instance()
                      .MetricsInterface()
                      .error(Firebolt::Metrics::ErrorType::Network, "ERR001", "Network error occurred", true,
                             std::map<std::string, std::string>{{"param1", "value1"}, {"param2", "value2"}},
                             Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::error() returned an error";
}

TEST_F(MetricsCTest, ErrorNoParameters)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().error(Firebolt::Metrics::ErrorType::Network,
                                                                                   "ERR001", "Network error occurred",
                                                                                   true, std::nullopt,
                                                                                   Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::error() returned an error";
}

TEST_F(MetricsCTest, ErrorNoAgePolicy)
{
    auto result = Firebolt::IFireboltAccessor::Instance()
                      .MetricsInterface()
                      .error(Firebolt::Metrics::ErrorType::Network, "ERR001", "Network error occurred", true,
                             std::map<std::string, std::string>{{"param1", "value1"}, {"param2", "value2"}},
                             std::nullopt);
    ASSERT_TRUE(result) << "MetricsImpl::error() returned an error";
}

TEST_F(MetricsCTest, MediaLoadStart)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaLoadStart("mediaEntity123",
                                                                                            Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaLoadStart() returned an error";
}

TEST_F(MetricsCTest, MediaPlay)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaPlay("mediaEntity123",
                                                                                       Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaPlay() returned an error";
}

TEST_F(MetricsCTest, MediaPlaying)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaPlaying("mediaEntity123",
                                                                                          Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaPlaying() returned an error";
}

TEST_F(MetricsCTest, MediaPause)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaPause("mediaEntity123",
                                                                                        Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaPause() returned an error";
}

TEST_F(MetricsCTest, MediaWaiting)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaWaiting("mediaEntity123",
                                                                                          Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaWaiting() returned an error";
}

TEST_F(MetricsCTest, MediaSeeking)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaSeeking("mediaEntity123", 0.5,
                                                                                          Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaSeeking() returned an error";
}

TEST_F(MetricsCTest, MediaSeekingInt)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaSeeking("mediaEntity123", 500,
                                                                                          Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaSeeking() returned an error";
}

TEST_F(MetricsCTest, MediaSeeked)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaSeeked("mediaEntity123", 0.5,
                                                                                         Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaSeeked() returned an error";
}

TEST_F(MetricsCTest, MediaSeekedInt)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaSeeked("mediaEntity123", 500,
                                                                                         Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaSeeked() returned an error";
}

TEST_F(MetricsCTest, MediaRateChanged)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaRateChanged("mediaEntity123", 1.5,
                                                                                              Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaRateChanged() returned an error";
}

TEST_F(MetricsCTest, MediaRenditionChanged)
{
    auto result =
        Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaRenditionChanged("mediaEntity123", 3000, 1920,
                                                                                         1080, "HDR",
                                                                                         Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaRenditionChanged() returned an error";
}

TEST_F(MetricsCTest, MediaRenditionChangedNoProfile)
{
    auto result =
        Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaRenditionChanged("mediaEntity123", 3000, 1920,
                                                                                         1080, std::nullopt,
                                                                                         Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaRenditionChanged() returned an error";
}

TEST_F(MetricsCTest, MediaRenditionChangedNoAgePolicy)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaRenditionChanged("mediaEntity123",
                                                                                                   3000, 1920, 1080,
                                                                                                   std::nullopt,
                                                                                                   std::nullopt);
    ASSERT_TRUE(result) << "MetricsImpl::mediaRenditionChanged() returned an error";
}

TEST_F(MetricsCTest, MediaEnded)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaEnded("mediaEntity123",
                                                                                        Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::mediaEnded() returned an error";
}

TEST_F(MetricsCTest, Event)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().event("https://com.example.event",
                                                                                   "{\"key\":\"value\"}",
                                                                                   Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "MetricsImpl::event() returned an error";
}

TEST_F(MetricsCTest, AppInfo)
{
    auto result = Firebolt::IFireboltAccessor::Instance().MetricsInterface().appInfo("1.0.0");
    ASSERT_TRUE(result) << "MetricsImpl::appInfo() returned an error";
}
