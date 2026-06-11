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

#include "firebolt/discovery.h"
#include "firebolt/firebolt.h"
#include "json_engine.h"
#include <gtest/gtest.h>

class DiscoveryCTest : public ::testing::Test
{
protected:
    JsonEngine jsonEngine;
};

TEST_F(DiscoveryCTest, Watched)
{
    auto result = Firebolt::IFireboltAccessor::Instance().DiscoveryInterface().watched("entity123", 0.75f, true,
                                                                                       "2024-10-01T12:00:00Z",
                                                                                       Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "Failed to call watched";
}

TEST_F(DiscoveryCTest, WatchedV2)
{
    auto expectedValue = jsonEngine.get_value("Discovery.watchedV2");
    auto result = Firebolt::IFireboltAccessor::Instance().DiscoveryInterface().watchedV2("entity123", 0.75f, true,
                                                                                         "2024-10-01T12:00:00Z",
                                                                                         Firebolt::AgePolicy::ADULT);
    ASSERT_TRUE(result) << "Failed to call watchedV2";
    EXPECT_EQ(*result, expectedValue.get<bool>());
}
