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

#include "json_types/metrics.h"
#include "metrics_impl.h"
#include "mock_helper.h"

class MetricsUTest : public ::testing::Test, protected MockBase
{
protected:
    Firebolt::Metrics::MetricsImpl metricsImpl_{mockHelper};
};

TEST_F(MetricsUTest, checkEnums)
{
    validate_enum("ErrorType", Firebolt::Metrics::JsonData::ErrorTypeEnum);
}

TEST_F(MetricsUTest, Ready)
{
    mock("Metrics.ready");
    auto result = metricsImpl_.ready();
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, SignIn)
{
    mock("Metrics.signIn");
    auto result = metricsImpl_.signIn();
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, SignOut)
{
    mock("Metrics.signOut");
    auto result = metricsImpl_.signOut();
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, StartContent)
{
    mock("Metrics.startContent");
    auto result = metricsImpl_.startContent("entity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, StopContent)
{
    mock("Metrics.stopContent");
    auto result = metricsImpl_.stopContent("entity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, Page)
{
    mock("Metrics.page");
    auto result = metricsImpl_.page("homePage", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, Error)
{
    mock("Metrics.error");
    auto result = metricsImpl_.error(Firebolt::Metrics::ErrorType::Network, "ERR001", "Network error occurred", true,
                                     std::map<std::string, std::string>{{"param1", "value1"}, {"param2", "value2"}},
                                     Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, ErrorNoParameters)
{
    mock("Metrics.error");
    auto result = metricsImpl_.error(Firebolt::Metrics::ErrorType::Network, "ERR001", "Network error occurred", true,
                                     std::nullopt, Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, ErrorNoAgePolicy)
{
    mock("Metrics.error");
    auto result = metricsImpl_.error(Firebolt::Metrics::ErrorType::Network, "ERR001", "Network error occurred", true,
                                     std::map<std::string, std::string>{{"param1", "value1"}, {"param2", "value2"}},
                                     std::nullopt);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaLoadStart)
{
    mock("Metrics.mediaLoadStart");
    auto result = metricsImpl_.mediaLoadStart("mediaEntity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaPlay)
{
    mock("Metrics.mediaPlay");
    auto result = metricsImpl_.mediaPlay("mediaEntity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaPlaying)
{
    mock("Metrics.mediaPlaying");
    auto result = metricsImpl_.mediaPlaying("mediaEntity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaPause)
{
    mock("Metrics.mediaPause");
    auto result = metricsImpl_.mediaPause("mediaEntity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaWaiting)
{
    mock("Metrics.mediaWaiting");
    auto result = metricsImpl_.mediaWaiting("mediaEntity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaSeeking)
{
    mock("Metrics.mediaSeeking");
    auto result = metricsImpl_.mediaSeeking("mediaEntity123", 0.5, Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaSeeked)
{
    mock("Metrics.mediaSeeked");
    auto result = metricsImpl_.mediaSeeked("mediaEntity123", 0.5, Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaRateChanged)
{
    mock("Metrics.mediaRateChanged");
    auto result = metricsImpl_.mediaRateChanged("mediaEntity123", 1.5, Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaRenditionChanged)
{
    mock("Metrics.mediaRenditionChanged");
    auto result =
        metricsImpl_.mediaRenditionChanged("mediaEntity123", 3000, 1920, 1080, "HDR", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaEnded)
{
    mock("Metrics.mediaEnded");
    auto result = metricsImpl_.mediaEnded("mediaEntity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, Event)
{
    mock("Metrics.event");
    auto result = metricsImpl_.event("https://com.example.schema", "{\"key\":\"value\"}", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, AppInfo)
{
    nlohmann::json expectedParams;
    expectedParams["build"] = "build123";
    EXPECT_CALL(mockHelper, invoke("Metrics.appInfo", expectedParams))
        .WillOnce(Invoke([&](const std::string& /*methodName*/, const nlohmann::json& /*parameters*/)
                         { return Firebolt::Result<void>{Firebolt::Error::None}; }));
    auto result = metricsImpl_.appInfo("build123");
    ASSERT_TRUE(result);
}
