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
    mockInvoke("Metrics.ready");
    auto result = metricsImpl_.ready();
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, SignIn)
{
    mockInvoke("Metrics.signIn");
    auto result = metricsImpl_.signIn();
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, SignOut)
{
    mockInvoke("Metrics.signOut");
    auto result = metricsImpl_.signOut();
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, StartContent)
{
    mockInvoke("Metrics.startContent");
    auto result = metricsImpl_.startContent("entity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, StopContent)
{
    mockInvoke("Metrics.stopContent");
    auto result = metricsImpl_.stopContent("entity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, Page)
{
    mockInvoke("Metrics.page");
    auto result = metricsImpl_.page("homePage", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, Error)
{
    mockInvoke("Metrics.error");
    auto result = metricsImpl_.error(Firebolt::Metrics::ErrorType::Network, "ERR001", "Network error occurred", true,
                                     std::map<std::string, std::string>{{"param1", "value1"}, {"param2", "value2"}},
                                     Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, ErrorNoParameters)
{
    mockInvoke("Metrics.error");
    auto result = metricsImpl_.error(Firebolt::Metrics::ErrorType::Network, "ERR001", "Network error occurred", true,
                                     std::nullopt, Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, ErrorNoAgePolicy)
{
    mockInvoke("Metrics.error");
    auto result = metricsImpl_.error(Firebolt::Metrics::ErrorType::Network, "ERR001", "Network error occurred", true,
                                     std::map<std::string, std::string>{{"param1", "value1"}, {"param2", "value2"}},
                                     std::nullopt);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaLoadStart)
{
    mockInvoke("Metrics.mediaLoadStart");
    auto result = metricsImpl_.mediaLoadStart("mediaEntity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaPlay)
{
    mockInvoke("Metrics.mediaPlay");
    auto result = metricsImpl_.mediaPlay("mediaEntity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaPlaying)
{
    mockInvoke("Metrics.mediaPlaying");
    auto result = metricsImpl_.mediaPlaying("mediaEntity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaPause)
{
    mockInvoke("Metrics.mediaPause");
    auto result = metricsImpl_.mediaPause("mediaEntity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaWaiting)
{
    mockInvoke("Metrics.mediaWaiting");
    auto result = metricsImpl_.mediaWaiting("mediaEntity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaSeeking)
{
    mockInvoke("Metrics.mediaSeeking");
    auto result = metricsImpl_.mediaSeeking("mediaEntity123", 0.5, Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaSeeked)
{
    mockInvoke("Metrics.mediaSeeked");
    auto result = metricsImpl_.mediaSeeked("mediaEntity123", 0.5, Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaRateChanged)
{
    mockInvoke("Metrics.mediaRateChanged");
    auto result = metricsImpl_.mediaRateChanged("mediaEntity123", 1.5, Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaRenditionChanged)
{
    mockInvoke("Metrics.mediaRenditionChanged");
    auto result =
        metricsImpl_.mediaRenditionChanged("mediaEntity123", 3000, 1920, 1080, "HDR", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, MediaEnded)
{
    mockInvoke("Metrics.mediaEnded");
    auto result = metricsImpl_.mediaEnded("mediaEntity123", Firebolt::AgePolicy::ADULT);
    EXPECT_TRUE(result);
}

TEST_F(MetricsUTest, EventRejectsInvalidJsonData)
{
    EXPECT_CALL(mockHelper, invoke("Metrics.event", _)).Times(0);

    for (const char* bad : {"", "foo", "{\"key\":", "{'key':'value'}"})
    {
        auto result = metricsImpl_.event("https://com.example.schema", bad, std::nullopt);
        ASSERT_FALSE(result) << bad;
        EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams) << bad;
    }
}

TEST_F(MetricsUTest, EventRejectsSchemaThatIsNotAUri)
{
    EXPECT_CALL(mockHelper, invoke("Metrics.event", _)).Times(0);

    for (const char* bad : {"", "no-scheme", ":missing", "scheme:", "1http://a", "ht tp://a", "http://a b", "ht!tp://a"})
    {
        auto result = metricsImpl_.event(bad, "{}", std::nullopt);
        ASSERT_FALSE(result) << bad;
        EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams) << bad;
    }
}

TEST_F(MetricsUTest, EventAcceptsUriSchemas)
{
    EXPECT_CALL(mockHelper, invoke("Metrics.event", _))
        .Times(3)
        .WillRepeatedly(::testing::Return(Firebolt::Result<void>{Firebolt::Error::None}));

    for (const char* good :
         {"https://com.example.schema", "http://meta.rdkcentral.com/some/schema", "urn:example:schema:1"})
    {
        EXPECT_TRUE(metricsImpl_.event(good, "{}", std::nullopt)) << good;
    }
}

TEST_F(MetricsUTest, ErrorRejectsUndefinedErrorType)
{
    EXPECT_CALL(mockHelper, invoke("Metrics.error", _)).Times(0);

    auto result = metricsImpl_.error(static_cast<Firebolt::Metrics::ErrorType>(99), "ERR001", "bad", true, std::nullopt,
                                     std::nullopt);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(MetricsUTest, EventAcceptsAnyValidJsonDocument)
{
    EXPECT_CALL(mockHelper, invoke("Metrics.event", _))
        .Times(4)
        .WillRepeatedly(::testing::Return(Firebolt::Result<void>{Firebolt::Error::None}));

    for (const char* good : {"{}", "[]", "\"text\"", "{\"key\":[1,2,{\"a\":null}]}"})
    {
        EXPECT_TRUE(metricsImpl_.event("https://com.example.schema", good, std::nullopt)) << good;
    }
}

TEST_F(MetricsUTest, Event)
{
    mockInvoke("Metrics.event");
    auto result = metricsImpl_.event("https://com.example.schema", R"({"key":"value"})", Firebolt::AgePolicy::ADULT);
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

// Payload tests: what is actually sent on the wire for each method (Firebolt 9 parameter names and optionality)
namespace
{
void expectPayload(MockHelper& helper, const std::string& method, const nlohmann::json& expected)
{
    EXPECT_CALL(helper, invoke(method, _))
        .WillOnce(Invoke(
            [expected](const std::string&, const nlohmann::json& parameters)
            {
                EXPECT_EQ(parameters, expected) << "got " << parameters.dump() << ", expected " << expected.dump();
                return Firebolt::Result<void>{Firebolt::Error::None};
            }));
}
} // namespace

TEST_F(MetricsUTest, ReadySignInSignOutSendNoParameters)
{
    for (const char* method : {"Metrics.ready", "Metrics.signIn", "Metrics.signOut"})
    {
        EXPECT_CALL(mockHelper, invoke(method, _))
            .WillOnce(Invoke(
                [](const std::string&, const nlohmann::json& parameters)
                {
                    EXPECT_TRUE(parameters.is_null() || parameters.empty());
                    return Firebolt::Result<void>{Firebolt::Error::None};
                }));
    }
    EXPECT_TRUE(metricsImpl_.ready());
    EXPECT_TRUE(metricsImpl_.signIn());
    EXPECT_TRUE(metricsImpl_.signOut());
}

TEST_F(MetricsUTest, StartAndStopContentPayload)
{
    expectPayload(mockHelper, "Metrics.startContent", nlohmann::json::object());
    EXPECT_TRUE(metricsImpl_.startContent(std::nullopt, std::nullopt));

    expectPayload(mockHelper, "Metrics.startContent", {{"entityId", "e1"}, {"agePolicy", "app:adult"}});
    EXPECT_TRUE(metricsImpl_.startContent("e1", Firebolt::AgePolicy::ADULT));

    expectPayload(mockHelper, "Metrics.stopContent", {{"entityId", "e1"}});
    EXPECT_TRUE(metricsImpl_.stopContent("e1", std::nullopt));

    expectPayload(mockHelper, "Metrics.stopContent", {{"agePolicy", "app:child"}});
    EXPECT_TRUE(metricsImpl_.stopContent(std::nullopt, Firebolt::AgePolicy::CHILD));
}

TEST_F(MetricsUTest, PagePayload)
{
    expectPayload(mockHelper, "Metrics.page", {{"pageId", "home"}});
    EXPECT_TRUE(metricsImpl_.page("home", std::nullopt));

    expectPayload(mockHelper, "Metrics.page", {{"pageId", "home"}, {"agePolicy", "app:teen"}});
    EXPECT_TRUE(metricsImpl_.page("home", Firebolt::AgePolicy::TEEN));
}

TEST_F(MetricsUTest, ErrorPayload)
{
    expectPayload(mockHelper, "Metrics.error",
                  {{"type", "network"}, {"code", "ERR001"}, {"description", "down"}, {"visible", true}});
    EXPECT_TRUE(
        metricsImpl_.error(Firebolt::Metrics::ErrorType::Network, "ERR001", "down", true, std::nullopt, std::nullopt));

    expectPayload(mockHelper, "Metrics.error",
                  {{"type", "other"},
                   {"code", "E2"},
                   {"description", "d"},
                   {"visible", false},
                   {"parameters", {{"a", "1"}, {"b", "2"}}},
                   {"agePolicy", "app:adult"}});
    EXPECT_TRUE(metricsImpl_.error(Firebolt::Metrics::ErrorType::Other, "E2", "d", false,
                                   std::map<std::string, std::string>{{"a", "1"}, {"b", "2"}},
                                   Firebolt::AgePolicy::ADULT));

    // An empty parameter map is still sent as an empty object, because the caller provided it
    expectPayload(mockHelper, "Metrics.error",
                  {{"type", "media"},
                   {"code", "E3"},
                   {"description", "d"},
                   {"visible", true},
                   {"parameters", nlohmann::json::object()}});
    EXPECT_TRUE(metricsImpl_.error(Firebolt::Metrics::ErrorType::Media, "E3", "d", true,
                                   std::map<std::string, std::string>{}, std::nullopt));
}

TEST_F(MetricsUTest, MediaEntityEventsPayload)
{
    const nlohmann::json withPolicy = {{"entityId", "m1"}, {"agePolicy", "app:adult"}};
    const nlohmann::json withoutPolicy = {{"entityId", "m1"}};
    expectPayload(mockHelper, "Metrics.mediaLoadStart", withoutPolicy);
    EXPECT_TRUE(metricsImpl_.mediaLoadStart("m1", std::nullopt));
    expectPayload(mockHelper, "Metrics.mediaPlay", withPolicy);
    EXPECT_TRUE(metricsImpl_.mediaPlay("m1", Firebolt::AgePolicy::ADULT));
    expectPayload(mockHelper, "Metrics.mediaPlaying", withoutPolicy);
    EXPECT_TRUE(metricsImpl_.mediaPlaying("m1", std::nullopt));
    expectPayload(mockHelper, "Metrics.mediaPause", withPolicy);
    EXPECT_TRUE(metricsImpl_.mediaPause("m1", Firebolt::AgePolicy::ADULT));
    expectPayload(mockHelper, "Metrics.mediaWaiting", withoutPolicy);
    EXPECT_TRUE(metricsImpl_.mediaWaiting("m1", std::nullopt));
    expectPayload(mockHelper, "Metrics.mediaEnded", withPolicy);
    EXPECT_TRUE(metricsImpl_.mediaEnded("m1", Firebolt::AgePolicy::ADULT));
}

TEST_F(MetricsUTest, MediaNumericEventsPayload)
{
    expectPayload(mockHelper, "Metrics.mediaSeeking", {{"entityId", "m1"}, {"target", 0.5}});
    EXPECT_TRUE(metricsImpl_.mediaSeeking("m1", 0.5, std::nullopt));

    expectPayload(mockHelper, "Metrics.mediaSeeked",
                  {{"entityId", "m1"}, {"position", 120.0}, {"agePolicy", "app:adult"}});
    EXPECT_TRUE(metricsImpl_.mediaSeeked("m1", 120.0, Firebolt::AgePolicy::ADULT));

    expectPayload(mockHelper, "Metrics.mediaRateChanged", {{"entityId", "m1"}, {"rate", 1.5}});
    EXPECT_TRUE(metricsImpl_.mediaRateChanged("m1", 1.5, std::nullopt));
}

TEST_F(MetricsUTest, MediaRenditionChangedPayload)
{
    expectPayload(mockHelper, "Metrics.mediaRenditionChanged",
                  {{"entityId", "m1"}, {"bitrate", 3000}, {"width", 1920}, {"height", 1080}});
    EXPECT_TRUE(metricsImpl_.mediaRenditionChanged("m1", 3000, 1920, 1080, std::nullopt, std::nullopt));

    expectPayload(mockHelper, "Metrics.mediaRenditionChanged",
                  {{"entityId", "m1"},
                   {"bitrate", 3000},
                   {"width", 1920},
                   {"height", 1080},
                   {"profile", "HDR"},
                   {"agePolicy", "app:adult"}});
    EXPECT_TRUE(metricsImpl_.mediaRenditionChanged("m1", 3000, 1920, 1080, "HDR", Firebolt::AgePolicy::ADULT));
}

TEST_F(MetricsUTest, EventAndAppInfoPayload)
{
    expectPayload(mockHelper, "Metrics.event", {{"schema", "https://com.example.schema"}, {"data", R"({"k":"v"})"}});
    EXPECT_TRUE(metricsImpl_.event("https://com.example.schema", R"({"k":"v"})", std::nullopt));

    expectPayload(mockHelper, "Metrics.event",
                  {{"schema", "https://com.example.schema"}, {"data", "{}"}, {"agePolicy", "app:teen"}});
    EXPECT_TRUE(metricsImpl_.event("https://com.example.schema", "{}", Firebolt::AgePolicy::TEEN));

    expectPayload(mockHelper, "Metrics.appInfo", {{"build", "1.2.3"}});
    EXPECT_TRUE(metricsImpl_.appInfo("1.2.3"));
}

TEST_F(MetricsUTest, PlatformErrorsPassThrough)
{
    EXPECT_CALL(mockHelper, invoke("Metrics.page", _))
        .WillOnce(::testing::Return(Firebolt::Result<void>{Firebolt::Error::General}));
    auto result = metricsImpl_.page("home", std::nullopt);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::General);
}
