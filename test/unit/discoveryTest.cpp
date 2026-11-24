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

#include "discovery_impl.h"
#include "json_engine.h"
#include "json_types/common.h"
#include "mock_helper.h"
#include <firebolt/json_types.h>
#include <limits>

class DiscoveryUTest : public ::testing::Test, protected MockBase
{
protected:
    Firebolt::Discovery::DiscoveryImpl discoveryImpl_{mockHelper};
};

TEST_F(DiscoveryUTest, checkEnums)
{
    validate_enum("AgePolicy", jsonEngine["x-schemas"]["Policies"]["AgePolicy"]["anyOf"][1]["enum"],
                  Firebolt::JsonData::AgePolicyEnum);
}

TEST_F(DiscoveryUTest, watched)
{
    mock("Discovery.watched");
    std::string entityId = "content123";
    std::optional<double> progress = 0.75F;
    std::optional<bool> completed = true;
    std::optional<std::string> watchedOn = "2024-06-01T12:00:00.000Z";
    std::optional<Firebolt::AgePolicy> agePolicy = Firebolt::AgePolicy::ADULT;
    auto result = discoveryImpl_.watched(entityId, progress, completed, watchedOn, agePolicy);
    ASSERT_TRUE(result) << "Error on watched";
    Firebolt::JSON::Boolean boolJson;
    boolJson.fromJson(jsonEngine.get_value("Discovery.watched"));
    EXPECT_EQ(boolJson.value(), *result);
}

TEST_F(DiscoveryUTest, watchedBadResponse)
{
    mock_with_response("Discovery.watched", "invalid_response");
    auto result = discoveryImpl_.watched("content123", std::nullopt, std::nullopt, std::nullopt, std::nullopt);
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(DiscoveryUTest, watched_payload)
{
    nlohmann::json expected;
    expected["entityId"] = "content123";
    expected["progress"] = 0.75F;
    expected["completed"] = true;
    expected["watchedOn"] = "2024-06-01T12:00:00.000Z";
    expected["agePolicy"] = "app:adult";
    EXPECT_CALL(mockHelper, getJson("Discovery.watched", _))
        .WillOnce(Invoke(
            [&](const std::string& /* methodName */, const nlohmann::json& parameters)
            {
                bool res = parameters == expected;
                EXPECT_EQ(parameters, expected) << "Parameters do not match expected payload: " << expected.dump()
                                                << " but got: " << parameters.dump();
                return Firebolt::Result<nlohmann::json>{nlohmann::json(res)};
            }));
    std::string entityId = "content123";
    std::optional<double> progress = 0.75F;
    std::optional<bool> completed = true;
    std::optional<std::string> watchedOn = "2024-06-01T12:00:00.000Z";
    std::optional<Firebolt::AgePolicy> agePolicy = Firebolt::AgePolicy::ADULT;
    auto result = discoveryImpl_.watched(entityId, progress, completed, watchedOn, agePolicy);
    ASSERT_TRUE(result) << "Error on watched";
    EXPECT_EQ(true, *result);
}

TEST_F(DiscoveryUTest, rejectsNegativeOrNonFiniteProgress)
{
    EXPECT_CALL(mockHelper, getJson("Discovery.watched", _)).Times(0);
    EXPECT_CALL(mockHelper, invoke("Discovery.watched", _)).Times(0);

    for (double bad : {-0.1, std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::infinity()})
    {
        auto watched = discoveryImpl_.watched("entity123", bad, true, std::nullopt, std::nullopt);
        ASSERT_FALSE(watched);
        EXPECT_EQ(watched.error(), Firebolt::Error::InvalidParams);

        auto watchedV2 = discoveryImpl_.watchedV2("entity123", bad, true, std::nullopt, std::nullopt);
        ASSERT_FALSE(watchedV2);
        EXPECT_EQ(watchedV2.error(), Firebolt::Error::InvalidParams);
    }
}

TEST_F(DiscoveryUTest, rejectsEmptyEntityId)
{
    EXPECT_CALL(mockHelper, getJson("Discovery.watched", _)).Times(0);
    EXPECT_CALL(mockHelper, invoke("Discovery.watched", _)).Times(0);

    auto watched = discoveryImpl_.watched("", std::nullopt, std::nullopt, std::nullopt, std::nullopt);
    ASSERT_FALSE(watched);
    EXPECT_EQ(watched.error(), Firebolt::Error::InvalidParams);

    auto watchedV2 = discoveryImpl_.watchedV2("", std::nullopt, std::nullopt, std::nullopt, std::nullopt);
    ASSERT_FALSE(watchedV2);
    EXPECT_EQ(watchedV2.error(), Firebolt::Error::InvalidParams);
}

TEST_F(DiscoveryUTest, rejectsMalformedWatchedOn)
{
    EXPECT_CALL(mockHelper, getJson("Discovery.watched", _)).Times(0);
    EXPECT_CALL(mockHelper, invoke("Discovery.watched", _)).Times(0);

    for (const char* bad :
         {// wrong shape: missing parts, whole seconds, other fraction lengths, offsets, separators, case, padding
          "", "2024-06-01", "2024-06-01T12:00:00Z", "2024-06-01T12:00:00.5Z", "2024-06-01T12:00:00.12Z",
          "2024-06-01T12:00:00.1234Z", "2024-06-01T12:00:00.000", "2024-06-01T12:00:00.000+00:00",
          "2024-06-01 12:00:00.000Z", "2024-06-01T12:00:00.000z", "2024-06-01T12:00:00.00aZ", "24-06-01T12:00:00.000Z",
          " 2024-06-01T12:00:00.000Z", "2024-06-01T12:00:00.000Z ", "2024-06-01T12:00:00,000Z",
          // right shape, field out of range
          "2024-13-01T00:00:00.000Z", "2024-00-10T00:00:00.000Z", "2024-06-00T00:00:00.000Z", "2024-06-32T00:00:00.000Z",
          "2024-06-01T24:00:00.000Z", "2024-06-01T12:60:00.000Z", "2024-06-01T12:00:60.000Z"})
    {
        auto watchedV2 =
            discoveryImpl_.watchedV2("entity123", std::nullopt, std::nullopt, std::string(bad), std::nullopt);
        ASSERT_FALSE(watchedV2) << bad;
        EXPECT_EQ(watchedV2.error(), Firebolt::Error::InvalidParams) << bad;
    }

    auto watched =
        discoveryImpl_.watched("entity123", std::nullopt, std::nullopt, std::string("not a date"), std::nullopt);
    ASSERT_FALSE(watched);
    EXPECT_EQ(watched.error(), Firebolt::Error::InvalidParams);
}

TEST_F(DiscoveryUTest, acceptsValidWatchedOn)
{
    const char* valid[] = {"2021-04-23T18:25:43.511Z", "2024-06-01T12:00:00.000Z", "2024-02-29T23:59:59.999Z",
                           "2000-02-29T00:00:00.000Z"};
    EXPECT_CALL(mockHelper, invoke("Discovery.watched", _))
        .Times(4)
        .WillRepeatedly(::testing::Return(Firebolt::Result<void>{Firebolt::Error::None}));

    for (const char* timestamp : valid)
    {
        auto result =
            discoveryImpl_.watchedV2("entity123", std::nullopt, std::nullopt, std::string(timestamp), std::nullopt);
        EXPECT_TRUE(result) << timestamp;
    }
}

TEST_F(DiscoveryUTest, acceptsLiveSecondsAsProgress)
{
    mockInvoke("Discovery.watched");
    auto result = discoveryImpl_.watchedV2("entity123", 3600.0, std::nullopt, std::nullopt, std::nullopt);
    ASSERT_TRUE(result);
}

TEST_F(DiscoveryUTest, watchedV2)
{
    mockInvoke("Discovery.watched");
    std::string entityId = "content123";
    std::optional<double> progress = 0.75F;
    std::optional<bool> completed = true;
    std::optional<std::string> watchedOn = "2024-06-01T12:00:00.000Z";
    std::optional<Firebolt::AgePolicy> agePolicy = Firebolt::AgePolicy::ADULT;
    auto result = discoveryImpl_.watchedV2(entityId, progress, completed, watchedOn, agePolicy);
    ASSERT_TRUE(result) << "Error on watchedV2";
}

TEST_F(DiscoveryUTest, watchedV2_payload)
{
    nlohmann::json expected;
    expected["entityId"] = "content123";
    expected["progress"] = 0.75F;
    expected["completed"] = true;
    expected["watchedOn"] = "2024-06-01T12:00:00.000Z";
    expected["agePolicy"] = "app:adult";
    EXPECT_CALL(mockHelper, invoke("Discovery.watched", _))
        .WillOnce(Invoke(
            [&](const std::string& /* methodName */, const nlohmann::json& parameters)
            {
                EXPECT_EQ(parameters, expected) << "Parameters do not match expected payload: " << expected.dump()
                                                << " but got: " << parameters.dump();
                return Firebolt::Result<void>{Firebolt::Error::None};
            }));
    std::string entityId = "content123";
    std::optional<double> progress = 0.75F;
    std::optional<bool> completed = true;
    std::optional<std::string> watchedOn = "2024-06-01T12:00:00.000Z";
    std::optional<Firebolt::AgePolicy> agePolicy = Firebolt::AgePolicy::ADULT;
    auto result = discoveryImpl_.watchedV2(entityId, progress, completed, watchedOn, agePolicy);
    ASSERT_TRUE(result) << "Error on watchedV2";
}
