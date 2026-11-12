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

#include "fake_subscriptions.h"
#include "firebolt_impl.h"
#include "mock_helper.h"
#include <firebolt/gateway.h>

using ::testing::_;
using ::testing::InSequence;
using ::testing::Return;
using ::testing::SaveArg;
using ::testing::Truly;

class MockGateway : public Firebolt::Transport::IGateway
{
public:
    MOCK_METHOD(Firebolt::Error, connect, (const Firebolt::Config&, Firebolt::Transport::ConnectionChangeCallback),
                (override));
    MOCK_METHOD(Firebolt::Error, disconnect, (), (override));
    MOCK_METHOD(std::future<Firebolt::Result<nlohmann::json>>, request, (const std::string&, const nlohmann::json&),
                (override));
    MOCK_METHOD(Firebolt::Error, subscribe, (const std::string&, Firebolt::Transport::EventCallback, void*), (override));
    MOCK_METHOD(Firebolt::Error, unsubscribe, (const std::string&, void*), (override));
    MOCK_METHOD(std::optional<std::string>, getResponseHeader, (const std::string&), (override));
    MOCK_METHOD(void, setResumeListener, (Firebolt::Transport::ResumeCallback), (override));
};

class FireboltAccessorUTest : public ::testing::Test, protected MockBase
{
protected:
    static bool isSet(const Firebolt::Transport::ResumeCallback& cb) { return static_cast<bool>(cb); }
    static bool isCleared(const Firebolt::Transport::ResumeCallback& cb) { return !cb; }

    ::testing::NiceMock<MockGateway> mockGateway;
    FakeSubscriptions fake_{mockHelper};
    Firebolt::FireboltAccessorImpl accessor_{mockGateway, mockHelper};
};

TEST_F(FireboltAccessorUTest, ConnectRegistersResumeListenerBeforeConnecting)
{
    InSequence seq;
    EXPECT_CALL(mockGateway, setResumeListener(Truly(isSet)));
    EXPECT_CALL(mockGateway, connect(_, _)).WillOnce(Return(Firebolt::Error::None));
    EXPECT_CALL(mockGateway, setResumeListener(Truly(isCleared)));
    EXPECT_CALL(mockGateway, disconnect()).WillOnce(Return(Firebolt::Error::None));

    EXPECT_EQ(accessor_.Connect(Firebolt::Config{}, nullptr), Firebolt::Error::None);
}

TEST_F(FireboltAccessorUTest, DisconnectClearsResumeListener)
{
    InSequence seq;
    EXPECT_CALL(mockGateway, setResumeListener(Truly(isCleared)));
    EXPECT_CALL(mockGateway, disconnect()).WillOnce(Return(Firebolt::Error::None));
    EXPECT_CALL(mockGateway, setResumeListener(Truly(isCleared)));
    EXPECT_CALL(mockGateway, disconnect()).WillOnce(Return(Firebolt::Error::None));

    EXPECT_EQ(accessor_.Disconnect(), Firebolt::Error::None);
}

TEST_F(FireboltAccessorUTest, DestructorClearsResumeListenerAndDisconnects)
{
    InSequence seq;
    EXPECT_CALL(mockGateway, setResumeListener(Truly(isCleared)));
    EXPECT_CALL(mockGateway, disconnect()).WillOnce(Return(Firebolt::Error::None));
}

TEST_F(FireboltAccessorUTest, ResumeListenerReconcilesLifecycle)
{
    using Firebolt::Lifecycle::LifecycleState;
    Firebolt::Transport::ResumeCallback listener;
    EXPECT_CALL(mockGateway, setResumeListener(Truly(isSet))).WillOnce(SaveArg<0>(&listener));
    EXPECT_CALL(mockGateway, setResumeListener(Truly(isCleared)));
    EXPECT_CALL(mockGateway, disconnect()).WillOnce(Return(Firebolt::Error::None));
    ON_CALL(mockGateway, connect(_, _)).WillByDefault(Return(Firebolt::Error::None));
    ASSERT_EQ(accessor_.Connect(Firebolt::Config{}, nullptr), Firebolt::Error::None);
    ASSERT_TRUE(listener);

    std::vector<Firebolt::Lifecycle::StateChange> last;
    auto id = accessor_.LifecycleInterface().subscribeOnStateChanged(
        [&last](const std::vector<Firebolt::Lifecycle::StateChange>& changes) { last = changes; });
    ASSERT_TRUE(id);
    fake_.emit("Lifecycle2.onStateChanged",
               nlohmann::json::array({{{"oldState", "initializing"}, {"newState", "active"}}}));

    mock_with_response("Lifecycle2.state", "hibernated");
    listener(std::chrono::milliseconds{2500});

    ASSERT_EQ(last.size(), 1u);
    EXPECT_EQ(last[0].oldState, LifecycleState::ACTIVE);
    EXPECT_EQ(last[0].newState, LifecycleState::HIBERNATED);
}
