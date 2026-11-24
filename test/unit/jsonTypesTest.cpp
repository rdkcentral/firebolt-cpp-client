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

#include "json_types/common.h"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

// Firebolt 9 `unsigned` is a 32-bit unsigned integer
TEST(StrictUnsignedTest, AcceptsTheFullRange)
{
    EXPECT_EQ(Firebolt::JsonData::parseUnsigned(nlohmann::json(0)), 0U);
    EXPECT_EQ(Firebolt::JsonData::parseUnsigned(nlohmann::json(42)), 42U);
    EXPECT_EQ(Firebolt::JsonData::parseUnsigned(nlohmann::json(4294967295U)), 4294967295U);
    EXPECT_EQ(Firebolt::JsonData::parseUnsigned(nlohmann::json(4294967295LL)), 4294967295U);
    EXPECT_EQ(Firebolt::JsonData::parseUnsigned(nlohmann::json::parse("7")), 7U);
}

TEST(StrictUnsignedTest, RejectsEverythingElse)
{
    for (const char* bad : {"-1", "-4294967295", "4294967296", "18446744073709551615", "1.5", "2.0", "0.0", "true",
                            "false", "\"5\"", "null", "[]", "{}"})
    {
        EXPECT_THROW(Firebolt::JsonData::parseUnsigned(nlohmann::json::parse(bad)), std::invalid_argument) << bad;
    }
}

// Byte counts (Stats) are 64-bit by design, so only the sign and the type are checked
TEST(StrictUnsignedTest, Unsigned64AcceptsLargeValuesAndRejectsBadOnes)
{
    EXPECT_EQ(Firebolt::JsonData::parseUnsigned64(nlohmann::json(0)), 0U);
    EXPECT_EQ(Firebolt::JsonData::parseUnsigned64(nlohmann::json(8589934592LL)), 8589934592ULL);
    EXPECT_EQ(Firebolt::JsonData::parseUnsigned64(nlohmann::json(18446744073709551615ULL)), 18446744073709551615ULL);

    for (const char* bad : {"-1", "-8589934592", "1.5", "2.0", "true", "\"5\"", "null", "[]"})
    {
        EXPECT_THROW(Firebolt::JsonData::parseUnsigned64(nlohmann::json::parse(bad)), std::invalid_argument) << bad;
    }
}

TEST(StrictUnsignedTest, ParserClassReturnsTheValue)
{
    Firebolt::JsonData::StrictUnsigned parser;
    parser.fromJson(nlohmann::json(9));
    EXPECT_EQ(parser.value(), 9U);
    EXPECT_THROW(parser.fromJson(nlohmann::json(-9)), std::invalid_argument);
}
