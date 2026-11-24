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

#include "display_impl.h"
#include "json_engine.h"
#include "mock_helper.h"
#include <vector>

class DisplayUTest : public ::testing::Test, protected MockBase
{
protected:
    Firebolt::Display::DisplayImpl displayImpl_{mockHelper};
};

TEST_F(DisplayUTest, Edid)
{
    mock("Display.edid");
    auto expectedValue = jsonEngine.get_value("Display.edid");
    auto result = displayImpl_.edid();
    ASSERT_TRUE(result) << "DisplayImpl::edid() returned an error";

    EXPECT_EQ(*result, expectedValue.get<std::string>());
}

TEST_F(DisplayUTest, EdidEmptyWhenNoDisplayConnected)
{
    mock_with_response("Display.edid", "");
    auto result = displayImpl_.edid();
    ASSERT_TRUE(result) << "DisplayImpl::edid() returned an error for an empty EDID";
    EXPECT_EQ(*result, "");
}

TEST_F(DisplayUTest, EdidBadResponse)
{
    mock_with_response("Display.edid", 12345);
    auto result = displayImpl_.edid();
    ASSERT_FALSE(result);
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(DisplayUTest, SizesRejectInvalidUnsignedValues)
{
    const std::vector<nlohmann::json> badUnsigned = {nlohmann::json(-1), nlohmann::json(4294967296LL),
                                                     nlohmann::json(1.5), nlohmann::json(true), nlohmann::json("5")};
    for (const auto& bad : badUnsigned)
    {
        mock_with_response("Display.size", nlohmann::json{{"width", bad}, {"height", 1}});
        auto size = displayImpl_.size();
        ASSERT_FALSE(size) << bad.dump();
        EXPECT_EQ(size.error(), Firebolt::Error::InvalidParams);

        mock_with_response("Display.maxResolution", nlohmann::json{{"width", 1}, {"height", bad}});
        auto max = displayImpl_.maxResolution();
        ASSERT_FALSE(max) << bad.dump();
        EXPECT_EQ(max.error(), Firebolt::Error::InvalidParams);
    }
}

TEST_F(DisplayUTest, MaxResolution)
{
    mock("Display.maxResolution");
    auto expectedValue = jsonEngine.get_value("Display.maxResolution");

    auto result = displayImpl_.maxResolution();
    ASSERT_TRUE(result) << "DisplayImpl::maxResolution() returned an error";

    EXPECT_EQ(result->height, expectedValue.at("height").get<uint32_t>());
    EXPECT_EQ(result->width, expectedValue.at("width").get<uint32_t>());
}

TEST_F(DisplayUTest, MaxResolutionBadResponse)
{
    mock_with_response("Display.maxResolution", "bad_response");
    auto result = displayImpl_.maxResolution();
    ASSERT_FALSE(result) << "DisplayImpl::maxResolution() did not return an error";
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}

TEST_F(DisplayUTest, Size)
{
    mock("Display.size");
    auto expectedValue = jsonEngine.get_value("Display.size");
    auto result = displayImpl_.size();
    ASSERT_TRUE(result) << "DisplayImpl::size() returned an error";

    EXPECT_EQ(result->height, expectedValue.at("height").get<uint32_t>());
    EXPECT_EQ(result->width, expectedValue.at("width").get<uint32_t>());
}

TEST_F(DisplayUTest, SizeBadResponse)
{
    mock_with_response("Display.size", "bad_response");
    auto result = displayImpl_.size();
    ASSERT_FALSE(result) << "DisplayImpl::size() did not return an error";
    EXPECT_EQ(result.error(), Firebolt::Error::InvalidParams);
}
