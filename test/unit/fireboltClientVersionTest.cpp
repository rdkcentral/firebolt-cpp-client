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

#include "mock_helper.h"
#include "firebolt_module_impl.h"
#include <firebolt/client_version.h>
#include <gtest/gtest.h>

TEST(FireboltClientVersionUTest, ClientVersionIsLocalAndBypassesTransport)
{
    ::testing::StrictMock<MockHelper> mockHelper;
    Firebolt::FireboltModule::FireboltImpl impl{mockHelper};

    EXPECT_CALL(mockHelper, unsubscribeAll(::testing::_)).Times(1);
    EXPECT_CALL(mockHelper, getJson(::testing::_, ::testing::_)).Times(0);

    auto result = impl.clientVersion();
    ASSERT_TRUE(result);
    EXPECT_EQ(*result, Firebolt::Version::ClientString);
}
