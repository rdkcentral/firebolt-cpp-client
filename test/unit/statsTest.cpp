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

#include "json_engine.h"
#include "mock_helper.h"
#include "stats_impl.h"

class StatsUTest : public ::testing::Test, protected MockBase
{
protected:
    Firebolt::Stats::StatsImpl statsImpl_{mockHelper};
};

TEST_F(StatsUTest, MemoryUsage)
{
    mock("Stats.memoryUsage");
    auto expectedValue = jsonEngine.get_value("Stats.memoryUsage");

    auto result = statsImpl_.memoryUsage();

    ASSERT_TRUE(result) << "StatsImpl::memoryUsage() returned an error";

    EXPECT_EQ(result->userMemoryUsed, expectedValue.at("userMemoryUsed").get<uint64_t>());
    EXPECT_EQ(result->userMemoryLimit, expectedValue.at("userMemoryLimit").get<uint64_t>());
    EXPECT_EQ(result->gpuMemoryUsed, expectedValue.at("gpuMemoryUsed").get<uint64_t>());
    EXPECT_EQ(result->gpuMemoryLimit, expectedValue.at("gpuMemoryLimit").get<uint64_t>());
}

TEST_F(StatsUTest, MemoryUsageAcceptsValuesAbove4GiB)
{
    mock_with_response("Stats.memoryUsage", {{"userMemoryUsed", 8589934592LL},
                                             {"userMemoryLimit", 17179869184LL},
                                             {"gpuMemoryUsed", 0},
                                             {"gpuMemoryLimit", 4294967296LL}});
    auto result = statsImpl_.memoryUsage();
    ASSERT_TRUE(result);
    EXPECT_EQ(result->userMemoryUsed, 8589934592ULL);
    EXPECT_EQ(result->userMemoryLimit, 17179869184ULL);
    EXPECT_EQ(result->gpuMemoryLimit, 4294967296ULL);
}

TEST_F(StatsUTest, MemoryUsageRejectsNegativeAndNonIntegerValues)
{
    for (const nlohmann::json& bad : {nlohmann::json(-1), nlohmann::json(1.5), nlohmann::json(true), nlohmann::json("5")})
    {
        mock_with_response("Stats.memoryUsage",
                           {{"userMemoryUsed", bad}, {"userMemoryLimit", 1}, {"gpuMemoryUsed", 1}, {"gpuMemoryLimit", 1}});
        auto result = statsImpl_.memoryUsage();
        ASSERT_FALSE(result) << bad.dump();
        EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
    }
}

TEST_F(StatsUTest, MemoryUsageBadResponse)
{
    mock_with_response("Stats.memoryUsage", "bad_response");
    auto result = statsImpl_.memoryUsage();
    ASSERT_FALSE(result) << "StatsImpl::memoryUsage() did not return an error";
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}
