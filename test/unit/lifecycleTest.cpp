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

#include <chrono>
#include <future>
#include <stdexcept>
#include <thread>

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

TEST_F(LifecycleReconcileUTest, UnsubscribeFailureStillClearsLocalSubscriberState)
{
    std::vector<Changes> received;
    subscribe(received);
    emit(change("initializing", "active"));

    EXPECT_CALL(mockHelper, unsubscribe(1)).WillOnce(Return(Firebolt::Result<void>{Firebolt::Error::General}));
    EXPECT_FALSE(lifecycleImpl_.unsubscribe(1));

    EXPECT_CALL(mockHelper, getJson("Lifecycle2.state", _)).Times(0);
    lifecycleImpl_.reconcile();
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

    mock_with_response("Lifecycle2.state", "paused");

    lifecycleImpl_.reconcile();

    ASSERT_EQ(received.size(), 2u);
    expectSingle(received.back(), Firebolt::Lifecycle::LifecycleState::ACTIVE,
                 Firebolt::Lifecycle::LifecycleState::PAUSED);
}

TEST_F(LifecycleReconcileUTest, CallbackCanWaitForAnotherThreadToUnsubscribe)
{
    using Firebolt::Lifecycle::LifecycleState;
    Firebolt::SubscriptionId id = 0;
    bool checkCallback = false;
    bool unsubscribeCompletedDuringCallback = false;
    std::thread unsubscribeThread;
    std::vector<Changes> received;
    auto result = lifecycleImpl_.subscribeOnStateChanged(
        [&](const Changes& changes)
        {
            received.push_back(changes);
            if (!checkCallback)
            {
                return;
            }

            std::promise<void> completed;
            auto completion = completed.get_future();
            unsubscribeThread = std::thread(
                [&, completed = std::move(completed)]() mutable
                {
                    (void)lifecycleImpl_.unsubscribe(id);
                    completed.set_value();
                });
            unsubscribeCompletedDuringCallback = completion.wait_for(std::chrono::seconds(1)) ==
                                                 std::future_status::ready;
        });
    ASSERT_TRUE(result);
    id = result.value();
    emit(change("initializing", "active"));

    checkCallback = true;
    mock_with_response("Lifecycle2.state", "suspended");
    lifecycleImpl_.reconcile();
    if (unsubscribeThread.joinable())
    {
        unsubscribeThread.join();
    }

    EXPECT_TRUE(unsubscribeCompletedDuringCallback);
    ASSERT_EQ(received.size(), 2u);
    expectSingle(received.back(), LifecycleState::ACTIVE, LifecycleState::SUSPENDED);
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

TEST_F(LifecycleReconcileUTest, DuplicatePrefixDoesNotDropFollowingRoundTrip)
{
    using Firebolt::Lifecycle::LifecycleState;
    std::vector<Changes> received;
    subscribe(received);
    emit(change("initializing", "active"));
    mock_with_response("Lifecycle2.state", "suspended");
    lifecycleImpl_.reconcile();

    emit(nlohmann::json::array({{{"oldState", "active"}, {"newState", "suspended"}},
                                {{"oldState", "suspended"}, {"newState", "paused"}},
                                {{"oldState", "paused"}, {"newState", "suspended"}}}));

    ASSERT_EQ(received.size(), 3u);
    ASSERT_EQ(received.back().size(), 2u);
    EXPECT_EQ(received.back()[0].oldState, LifecycleState::SUSPENDED);
    EXPECT_EQ(received.back()[0].newState, LifecycleState::PAUSED);
    EXPECT_EQ(received.back()[1].oldState, LifecycleState::PAUSED);
    EXPECT_EQ(received.back()[1].newState, LifecycleState::SUSPENDED);
}

TEST_F(LifecycleReconcileUTest, CorrectionTokenExpiresAfterFirstNonmatchingTransition)
{
    using Firebolt::Lifecycle::LifecycleState;
    std::vector<Changes> received;
    subscribe(received);
    emit(change("initializing", "active"));
    mock_with_response("Lifecycle2.state", "suspended");
    lifecycleImpl_.reconcile();

    emit(nlohmann::json::array(
        {{{"oldState", "suspended"}, {"newState", "active"}}, {{"oldState", "active"}, {"newState", "suspended"}}}));

    ASSERT_EQ(received.size(), 3u);
    ASSERT_EQ(received.back().size(), 2u);
    EXPECT_EQ(received.back()[0].oldState, LifecycleState::SUSPENDED);
    EXPECT_EQ(received.back()[0].newState, LifecycleState::ACTIVE);
    EXPECT_EQ(received.back()[1].oldState, LifecycleState::ACTIVE);
    EXPECT_EQ(received.back()[1].newState, LifecycleState::SUSPENDED);
}

TEST_F(LifecycleReconcileUTest, PendingCorrectionsSurviveRepeatedResumes)
{
    std::vector<Changes> received;
    subscribe(received);
    emit(change("initializing", "active"));
    mock_with_response("Lifecycle2.state", "suspended");
    lifecycleImpl_.reconcile();
    mock_with_response("Lifecycle2.state", "paused");
    lifecycleImpl_.reconcile();

    emit(nlohmann::json::array(
        {{{"oldState", "active"}, {"newState", "suspended"}}, {{"oldState", "suspended"}, {"newState", "paused"}}}));

    EXPECT_EQ(received.size(), 3u);
}

TEST_F(LifecycleReconcileUTest, NewTransitionIsNotSuppressedAfterStateReturnsToCorrectionOrigin)
{
    using Firebolt::Lifecycle::LifecycleState;
    std::vector<Changes> received;
    subscribe(received);
    emit(change("initializing", "active"));
    mock_with_response("Lifecycle2.state", "suspended");
    lifecycleImpl_.reconcile();
    mock_with_response("Lifecycle2.state", "active");
    lifecycleImpl_.reconcile();

    emit(change("active", "suspended"));

    ASSERT_EQ(received.size(), 4u);
    expectSingle(received.back(), LifecycleState::ACTIVE, LifecycleState::SUSPENDED);
}

TEST_F(LifecycleReconcileUTest, DelayedCorrectionIsSuppressedWhenEarlierCorrectionNeverArrives)
{
    std::vector<Changes> received;
    subscribe(received);
    emit(change("initializing", "active"));
    mock_with_response("Lifecycle2.state", "suspended");
    lifecycleImpl_.reconcile();
    mock_with_response("Lifecycle2.state", "active");
    lifecycleImpl_.reconcile();

    emit(change("suspended", "active"));

    EXPECT_EQ(received.size(), 3u);
}

TEST_F(LifecycleReconcileUTest, LaterCorrectionMatchesAfterEarlierWireNotificationWasDropped)
{
    std::vector<Changes> received;
    subscribe(received);
    emit(change("initializing", "active"));
    mock_with_response("Lifecycle2.state", "suspended");
    lifecycleImpl_.reconcile();
    mock_with_response("Lifecycle2.state", "paused");
    lifecycleImpl_.reconcile();

    emit(change("suspended", "paused"));

    EXPECT_EQ(received.size(), 3u);
}

TEST_F(LifecycleReconcileUTest, CallbackExceptionDoesNotPreventOtherSubscribersFromReceivingCorrection)
{
    std::vector<Changes> received;
    int throwingCallbackCalls = 0;
    auto throwingId = lifecycleImpl_.subscribeOnStateChanged(
        [&throwingCallbackCalls](const Changes&)
        {
            ++throwingCallbackCalls;
            throw std::runtime_error("subscriber failure");
        });
    ASSERT_TRUE(throwingId);
    subscribe(received);
    emit(change("initializing", "active"));

    mock_with_response("Lifecycle2.state", "suspended");
    lifecycleImpl_.reconcile();

    ASSERT_EQ(received.size(), 2u);
    EXPECT_EQ(throwingCallbackCalls, 2);
    emit(change("active", "suspended"));
    EXPECT_EQ(received.size(), 2u);
    EXPECT_EQ(throwingCallbackCalls, 2);
}

TEST_F(LifecycleReconcileUTest, UnsubscribeAllCancelsInFlightSubscription)
{
    std::promise<void> subscribeEntered;
    auto subscribeEnteredFuture = subscribeEntered.get_future();
    std::promise<void> allowSubscribeToFinish;
    auto allowSubscribeToFinishFuture = allowSubscribeToFinish.get_future();
    std::optional<Firebolt::Result<Firebolt::SubscriptionId>> result;

    EXPECT_CALL(mockHelper, subscribe(_, "Lifecycle2.onStateChanged", _, _))
        .WillOnce(Invoke(
            [&](void* /*owner*/, const std::string& /*eventName*/, std::any&& /*notification*/,
                void (* /*callback*/)(void*, const nlohmann::json&))
            {
                subscribeEntered.set_value();
                allowSubscribeToFinishFuture.wait();
                return Firebolt::Result<Firebolt::SubscriptionId>{1};
            }));
    EXPECT_CALL(mockHelper, unsubscribeAll(_)).Times(::testing::AtLeast(1));

    std::thread subscribeThread([&] { result.emplace(lifecycleImpl_.subscribeOnStateChanged([](const Changes&) {})); });
    const auto waitResult = subscribeEnteredFuture.wait_for(std::chrono::seconds(1));
    EXPECT_EQ(waitResult, std::future_status::ready);
    std::promise<void> unsubscribeStarted;
    auto unsubscribeStartedFuture = unsubscribeStarted.get_future();
    std::promise<void> unsubscribeFinished;
    auto unsubscribeFinishedFuture = unsubscribeFinished.get_future();
    std::thread unsubscribeThread;
    if (waitResult == std::future_status::ready)
    {
        unsubscribeThread = std::thread(
            [&]
            {
                unsubscribeStarted.set_value();
                lifecycleImpl_.unsubscribeAll();
                unsubscribeFinished.set_value();
            });
        EXPECT_EQ(unsubscribeStartedFuture.wait_for(std::chrono::seconds(1)), std::future_status::ready);
        EXPECT_EQ(unsubscribeFinishedFuture.wait_for(std::chrono::milliseconds(10)), std::future_status::timeout);
    }
    allowSubscribeToFinish.set_value();
    subscribeThread.join();
    if (unsubscribeThread.joinable())
    {
        unsubscribeThread.join();
    }

    ASSERT_TRUE(result.has_value());
    EXPECT_TRUE(*result);
    EXPECT_CALL(mockHelper, getJson("Lifecycle2.state", _)).Times(0);
    lifecycleImpl_.reconcile();
}

TEST_F(LifecycleReconcileUTest, RoundTripToLastSeenStateWithoutCorrectionIsDelivered)
{
    using Firebolt::Lifecycle::LifecycleState;
    std::vector<Changes> received;
    subscribe(received);
    emit(change("initializing", "active"));

    mock_with_response("Lifecycle2.state", "active");
    lifecycleImpl_.reconcile();
    emit(change("hibernated", "active"));

    ASSERT_EQ(received.size(), 2u);
    expectSingle(received.back(), LifecycleState::HIBERNATED, LifecycleState::ACTIVE);
}

TEST_F(LifecycleReconcileUTest, UnsubscribeAndResubscribeClearsStaleReconciliationState)
{
    using Firebolt::Lifecycle::LifecycleState;
    std::vector<Changes> original;
    auto originalId =
        lifecycleImpl_.subscribeOnStateChanged([&original](const Changes& changes) { original.push_back(changes); });
    ASSERT_TRUE(originalId);
    emit(change("initializing", "active"));

    ASSERT_TRUE(lifecycleImpl_.unsubscribe(originalId.value()));
    EXPECT_CALL(mockHelper, getJson("Lifecycle2.state", _)).Times(0);
    lifecycleImpl_.reconcile();

    std::vector<Changes> replacement;
    subscribe(replacement);
    lifecycleImpl_.reconcile();
    EXPECT_TRUE(replacement.empty());

    emit(change("suspended", "paused"));
    ASSERT_EQ(replacement.size(), 1u);
    ASSERT_EQ(replacement.back().size(), 1u);
    EXPECT_EQ(replacement.back()[0].oldState, LifecycleState::SUSPENDED);
    EXPECT_EQ(replacement.back()[0].newState, LifecycleState::PAUSED);
}

TEST_F(LifecycleReconcileUTest, UnsubscribeAllClearsStaleReconciliationState)
{
    std::vector<Changes> original;
    subscribe(original);
    emit(change("initializing", "active"));

    lifecycleImpl_.unsubscribeAll();
    EXPECT_CALL(mockHelper, getJson("Lifecycle2.state", _)).Times(0);
    lifecycleImpl_.reconcile();

    std::vector<Changes> replacement;
    subscribe(replacement);
    lifecycleImpl_.reconcile();
    EXPECT_TRUE(replacement.empty());
}
