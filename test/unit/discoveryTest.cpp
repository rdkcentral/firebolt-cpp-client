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
#include "mock_helper.h"
#include <firebolt/json_types.h>

class DiscoveryTest : public ::testing::Test, protected MockBase
{
protected:
    Firebolt::Discovery::DiscoveryImpl discoveryImpl_{mockHelper};
};

TEST_F(DiscoveryTest, watched)
{
    mock("Discovery.watched");
    std::string entityId = "content123";
    std::optional<float> progress = 0.75f;
    std::optional<bool> completed = true;
    std::optional<std::string> watchedOn = "2024-06-01T12:00:00Z";
    std::optional<Firebolt::Discovery::AgePolicy> agePolicy = Firebolt::Discovery::AgePolicy::ADULT;
    auto result = discoveryImpl_.watched(entityId, progress, completed, watchedOn, agePolicy);
    ASSERT_TRUE(result) << "Error on watched";
    Firebolt::JSON::Boolean boolJson;
    boolJson.fromJson(jsonEngine.get_value("Discovery.watched"));
    EXPECT_EQ(boolJson.value(), *result);
}

TEST_F(DiscoveryTest, watched_payload)
{
    nlohmann::json expected;
    expected["entityId"] = "content123";
    expected["progress"] = 0.75f;
    expected["completed"] = true;
    expected["watchedOn"] = "2024-06-01T12:00:00Z";
    expected["agePolicy"] = "app:adult";
    EXPECT_CALL(mockHelper, getJson("Discovery.watched", _))
        .WillOnce(Invoke(
            [&](const std::string& /* methodName */, const nlohmann::json& parameters)
            {
                bool res = parameters == expected;
                EXPECT_EQ(parameters, expected) << "Parameters do not match expected payload: " << expected.dump()
                                                << " but got: " << parameters.dump();
                return Firebolt::Result<nlohmann::json>{res};
            }));
    std::string entityId = "content123";
    std::optional<float> progress = 0.75f;
    std::optional<bool> completed = true;
    std::optional<std::string> watchedOn = "2024-06-01T12:00:00Z";
    std::optional<Firebolt::Discovery::AgePolicy> agePolicy = Firebolt::Discovery::AgePolicy::ADULT;
    auto result = discoveryImpl_.watched(entityId, progress, completed, watchedOn, agePolicy);
    ASSERT_TRUE(result) << "Error on watched";
    EXPECT_EQ(true, *result);
}
