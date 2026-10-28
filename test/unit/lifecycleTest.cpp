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

#include "json_types/lifecycle.h"
#include "fake_subscriptions.h"
#include "lifecycle_impl.h"
#include "mock_helper.h"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

class LifecycleUTest : public ::testing::Test, protected MockBase
{
protected:
    void SetUp() override {}

    void TearDown() override
    {
        EXPECT_CALL(mockHelper, unsubscribeAll(_))
            .WillRepeatedly(Invoke([](auto) { return Firebolt::Result<void>(Firebolt::Error::None); }));
    }

    Firebolt::Lifecycle::LifecycleImpl lifecycleImpl_{mockHelper};
};

TEST_F(LifecycleUTest, checkEnums)
{
    validate_enum("CloseType", Firebolt::Lifecycle::JsonData::CloseReasonEnum);
    validate_enum("LifecycleState", Firebolt::Lifecycle::JsonData::LifecycleStateEnum);
}

TEST_F(LifecycleUTest, close)
{
    nlohmann::json expectedParams;
    expectedParams["type"] = "deactivate";
    EXPECT_CALL(mockHelper, invoke("Lifecycle2.close", expectedParams))
        .WillOnce(Invoke([&](const std::string& /*methodName*/, const nlohmann::json& /*parameters*/)
                         { return Firebolt::Result<void>{Firebolt::Error::None}; }));

    auto result = lifecycleImpl_.close(Firebolt::Lifecycle::CloseType::DEACTIVATE);
    ASSERT_TRUE(result) << "Error on invoke";
}

TEST_F(LifecycleUTest, state)
{
    mock_with_response("Lifecycle2.state", "initializing");
    Firebolt::Result<Firebolt::Lifecycle::LifecycleState> result = lifecycleImpl_.state();
    ASSERT_TRUE(result) << "Failed to retrieve current state from Lifecycle.state() method";
    EXPECT_EQ(*result, Firebolt::Lifecycle::LifecycleState::INITIALIZING);
}

TEST_F(LifecycleUTest, stateBadResponse)
{
    mock_with_response("Lifecycle2.state", "invalid_response");
    Firebolt::Result<Firebolt::Lifecycle::LifecycleState> result = lifecycleImpl_.state();
    ASSERT_FALSE(result) << "LifecycleImpl::state() did not return an error";
}

TEST_F(LifecycleUTest, subscribeOnStateChanged)
{
    EXPECT_CALL(mockHelper, subscribe(_, "Lifecycle2.onStateChanged", _, _))
        .WillOnce(Invoke([&](void* /*owner*/, const std::string& /*eventName*/, std::any&& /*notification*/,
                             void (* /*callback*/)(void*, const nlohmann::json&))
                         { return Firebolt::Result<Firebolt::SubscriptionId>{1}; }));

    EXPECT_CALL(mockHelper, unsubscribe(1)).WillOnce(Return(Firebolt::Result<void>{Firebolt::Error::None}));

    auto id =
        lifecycleImpl_.subscribeOnStateChanged([](const std::vector<Firebolt::Lifecycle::StateChange>& /*changes*/) {});

    ASSERT_TRUE(id) << "error on subscribe ";
    EXPECT_EQ(id.value(), 1);

    auto result = lifecycleImpl_.unsubscribe(id.value());
    ASSERT_TRUE(result) << "error on unsubscribe";
}

class LifecycleReconcileUTest : public ::testing::Test, protected MockBase
{
protected:
    using Changes = std::vector<Firebolt::Lifecycle::StateChange>;

    void subscribe(std::vector<Changes>& sink)
    {
        auto id = lifecycleImpl_.subscribeOnStateChanged([&sink](const Changes& changes) { sink.push_back(changes); });
        ASSERT_TRUE(id) << "error on subscribe";
    }

    void emit(const nlohmann::json& changes) { fake_.emit("Lifecycle2.onStateChanged", changes); }

    static nlohmann::json change(const std::string& oldState, const std::string& newState)
    {
        return nlohmann::json::array({{{"oldState", oldState}, {"newState", newState}}});
    }

    static void expectSingle(const Changes& changes, Firebolt::Lifecycle::LifecycleState oldState,
                             Firebolt::Lifecycle::LifecycleState newState)
    {
        ASSERT_EQ(changes.size(), 1u);
        EXPECT_EQ(changes[0].oldState, oldState);
        EXPECT_EQ(changes[0].newState, newState);
    }

    FakeSubscriptions fake_{mockHelper};
    Firebolt::Lifecycle::LifecycleImpl lifecycleImpl_{mockHelper};
};

TEST_F(LifecycleReconcileUTest, ChangedStateDeliveredOnceToEachSubscriber)
{
    using Firebolt::Lifecycle::LifecycleState;
    std::vector<Changes> first;
    std::vector<Changes> second;
    subscribe(first);
    subscribe(second);
    emit(change("initializing", "active"));

    mock_with_response("Lifecycle2.state", "suspended");
    lifecycleImpl_.reconcile();

    for (const auto* sink : {&first, &second})
    {
        ASSERT_EQ(sink->size(), 2u);
        expectSingle(sink->back(), LifecycleState::ACTIVE, LifecycleState::SUSPENDED);
    }
}

TEST_F(LifecycleReconcileUTest, UnchangedStateDeliversNothing)
{
    std::vector<Changes> received;
    subscribe(received);
    emit(change("initializing", "active"));

    mock_with_response("Lifecycle2.state", "active");
    lifecycleImpl_.reconcile();

    EXPECT_EQ(received.size(), 1u);
}

TEST_F(LifecycleReconcileUTest, NoSubscribersMakesNoRequest)
{
    EXPECT_CALL(mockHelper, getJson("Lifecycle2.state", _)).Times(0);
    EXPECT_CALL(mockHelper, dispatchLocal(_, _, _)).Times(0);

    lifecycleImpl_.reconcile();
}

TEST_F(LifecycleReconcileUTest, UnknownLastStateMakesNoRequest)
{
    std::vector<Changes> received;
    subscribe(received);
    EXPECT_CALL(mockHelper, getJson("Lifecycle2.state", _)).Times(0);

    lifecycleImpl_.reconcile();

    EXPECT_TRUE(received.empty());
}

TEST_F(LifecycleReconcileUTest, StateQueryErrorDeliversNothingAndKeepsLastState)
{
    using Firebolt::Lifecycle::LifecycleState;
    std::vector<Changes> received;
    subscribe(received);
    emit(change("initializing", "active"));

    EXPECT_CALL(mockHelper, getJson("Lifecycle2.state", _))
        .WillOnce(Return(Firebolt::Result<nlohmann::json>{Firebolt::Error::Timedout}))
        .WillOnce(Return(Firebolt::Result<nlohmann::json>{nlohmann::json("suspended")}));

    lifecycleImpl_.reconcile();
    EXPECT_EQ(received.size(), 1u);

    lifecycleImpl_.reconcile();
    ASSERT_EQ(received.size(), 2u);
    expectSingle(received.back(), LifecycleState::ACTIVE, LifecycleState::SUSPENDED);
}

TEST_F(LifecycleReconcileUTest, ResumeListenerTargetsOnlyLifecycleSubscribers)
{
    std::vector<Changes> received;
    subscribe(received);
    emit(change("initializing", "active"));

    EXPECT_CALL(mockHelper, dispatchLocal(&lifecycleImpl_, "Lifecycle2.onStateChanged", _)).Times(1);
    mock_with_response("Lifecycle2.state", "paused");

    lifecycleImpl_.reconcile();
}

TEST_F(LifecycleReconcileUTest, HeldEventDuplicatingSynthesizedChangeDeliveredOnce)
{
    using Firebolt::Lifecycle::LifecycleState;
    std::vector<Changes> received;
    subscribe(received);
    emit(change("initializing", "active"));
    mock_with_response("Lifecycle2.state", "suspended");
    lifecycleImpl_.reconcile();

    emit(change("active", "suspended"));

    ASSERT_EQ(received.size(), 2u);
    expectSingle(received.back(), LifecycleState::ACTIVE, LifecycleState::SUSPENDED);
}

TEST_F(LifecycleReconcileUTest, DuplicateSuppressedForEverySubscriber)
{
    std::vector<Changes> first;
    std::vector<Changes> second;
    subscribe(first);
    subscribe(second);
    emit(change("initializing", "active"));
    mock_with_response("Lifecycle2.state", "suspended");
    lifecycleImpl_.reconcile();

    emit(change("active", "suspended"));

    EXPECT_EQ(first.size(), 2u);
    EXPECT_EQ(second.size(), 2u);
}

TEST_F(LifecycleReconcileUTest, RoundTripFromLastDeliveredStateIsDelivered)
{
    std::vector<Changes> received;
    subscribe(received);
    emit(change("initializing", "active"));

    emit(nlohmann::json::array(
        {{{"oldState", "active"}, {"newState", "paused"}}, {{"oldState", "paused"}, {"newState", "active"}}}));

    ASSERT_EQ(received.size(), 2u);
    EXPECT_EQ(received.back().size(), 2u);
}
