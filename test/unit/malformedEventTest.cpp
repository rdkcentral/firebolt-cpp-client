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

#include "accessibility_impl.h"
#include "actions_impl.h"
#include "device_impl.h"
#include "fake_subscriptions.h"
#include "lifecycle_impl.h"
#include "localization_impl.h"
#include "network_impl.h"
#include "presentation_impl.h"
#include "speechsynthesis_impl.h"
#include "texttospeech_impl.h"
#include "videooutput_impl.h"

// Rule for every module that has events: a malformed event payload is dropped, the caller's callback is not called,
// the subscription stays active, and a later well-formed event is delivered. The parsing and dropping is done by the
// transport library; these tests keep the client's parsers honest.
class MalformedEventUTest : public ::testing::Test
{
protected:
    ::testing::NiceMock<MockHelper> mockHelper;
    FakeSubscriptions fake{mockHelper};

    template <typename Subscribe>
    void expectMalformedDropped(const std::string& event, const nlohmann::json& bad, const nlohmann::json& good,
                                Subscribe subscribe)
    {
        SCOPED_TRACE(event);
        int calls = 0;
        auto id = subscribe([&calls](auto&&...) { ++calls; });
        ASSERT_TRUE(id);

        fake.emit(event, bad);
        EXPECT_EQ(calls, 0) << "callback must not be called for a malformed payload";
        EXPECT_EQ(fake.count(), 1U) << "subscription must stay active";

        fake.emit(event, good);
        EXPECT_EQ(calls, 1) << "a well-formed payload must still be delivered";
    }
};

TEST_F(MalformedEventUTest, Accessibility)
{
    Firebolt::Accessibility::AccessibilityImpl impl{mockHelper};
    expectMalformedDropped("Accessibility.onAudioDescriptionChanged", "yes", true,
                           [&](auto cb) { return impl.subscribeOnAudioDescriptionChanged(std::move(cb)); });
    expectMalformedDropped("Accessibility.onHighContrastUIChanged", 5, false,
                           [&](auto cb) { return impl.subscribeOnHighContrastUIChanged(std::move(cb)); });
    expectMalformedDropped("Accessibility.onClosedCaptionsSettingsChanged", nlohmann::json{{"enabled", true}},
                           nlohmann::json{{"enabled", true}, {"preferredLanguages", {"eng"}}},
                           [&](auto cb) { return impl.subscribeOnClosedCaptionsSettingsChanged(std::move(cb)); });
    expectMalformedDropped("Accessibility.onVoiceGuidanceSettingsChanged", nlohmann::json{{"enabled", true}},
                           nlohmann::json{{"enabled", true}, {"rate", 1.0}, {"navigationHints", true}},
                           [&](auto cb) { return impl.subscribeOnVoiceGuidanceSettingsChanged(std::move(cb)); });
}

TEST_F(MalformedEventUTest, Actions)
{
    Firebolt::Actions::ActionsImpl impl{mockHelper};
    expectMalformedDropped("Actions.onIntent", nlohmann::json{{"intentId", 1}},
                           nlohmann::json{{"intent", {{"action", "pre-load"}}}, {"intentId", 1}},
                           [&](auto cb) { return impl.subscribeOnIntent(std::move(cb)); });
}

TEST_F(MalformedEventUTest, Device)
{
    Firebolt::Device::DeviceImpl impl{mockHelper};
    expectMalformedDropped("Device.onHdrChanged", nlohmann::json{{"hdr10", true}},
                           nlohmann::json{{"hdr10", true}, {"hdr10Plus", false}, {"dolbyVision", false}, {"hlg", true}},
                           [&](auto cb) { return impl.subscribeOnHdrChanged(std::move(cb)); });
    expectMalformedDropped("Device.onDolbyAtmosExperienceAvailableChanged", "yes", true, [&](auto cb)
                           { return impl.subscribeOnDolbyAtmosExperienceAvailableChanged(std::move(cb)); });
    expectMalformedDropped("Device.onNameChanged", 123, "Living Room TV",
                           [&](auto cb) { return impl.subscribeOnNameChanged(std::move(cb)); });
}

TEST_F(MalformedEventUTest, Lifecycle)
{
    Firebolt::Lifecycle::LifecycleImpl impl{mockHelper};
    expectMalformedDropped("Lifecycle2.onStateChanged",
                           nlohmann::json::array({{{"oldState", "bogus"}, {"newState", "paused"}}}),
                           nlohmann::json::array({{{"oldState", "initializing"}, {"newState", "paused"}}}),
                           [&](auto cb) { return impl.subscribeOnStateChanged(std::move(cb)); });
}

TEST_F(MalformedEventUTest, Localization)
{
    Firebolt::Localization::LocalizationImpl impl{mockHelper};
    expectMalformedDropped("Localization.onCountryChanged", 123, "US",
                           [&](auto cb) { return impl.subscribeOnCountryChanged(std::move(cb)); });
    expectMalformedDropped("Localization.onPreferredAudioLanguagesChanged", "eng", nlohmann::json::array({"eng", "spa"}),
                           [&](auto cb) { return impl.subscribeOnPreferredAudioLanguagesChanged(std::move(cb)); });
    expectMalformedDropped("Localization.onPresentationLanguageChanged", 123, "en-US",
                           [&](auto cb) { return impl.subscribeOnPresentationLanguageChanged(std::move(cb)); });
    expectMalformedDropped("Localization.onTimeZoneChanged", 123, "America/New_York",
                           [&](auto cb) { return impl.subscribeOnTimeZoneChanged(std::move(cb)); });
}

TEST_F(MalformedEventUTest, Network)
{
    Firebolt::Network::NetworkImpl impl{mockHelper};
    expectMalformedDropped("Network.onConnectedChanged", "yes", true,
                           [&](auto cb) { return impl.subscribeOnConnectedChanged(std::move(cb)); });
}

TEST_F(MalformedEventUTest, Presentation)
{
    Firebolt::Presentation::PresentationImpl impl{mockHelper};
    expectMalformedDropped("Presentation.onFocusedChanged", "yes", true,
                           [&](auto cb) { return impl.subscribeOnFocusedChanged(std::move(cb)); });
}

TEST_F(MalformedEventUTest, SpeechSynthesis)
{
    Firebolt::SpeechSynthesis::SpeechSynthesisImpl impl{mockHelper};
    expectMalformedDropped("SpeechSynthesis.onVoicesChanged", nlohmann::json{{"name", "not-an-array"}},
                           nlohmann::json::array({{{"name", "Salli"}, {"lang", "en-US"}, {"default", true}}}),
                           [&](auto cb) { return impl.subscribeOnVoicesChanged(std::move(cb)); });
    expectMalformedDropped("SpeechSynthesis.onUtteranceEvent", nlohmann::json{{"utteranceId", 1}, {"event", "bogus"}},
                           nlohmann::json{{"utteranceId", 1}, {"event", "completed"}},
                           [&](auto cb) { return impl.subscribeOnUtteranceEvent(std::move(cb)); });
}

TEST_F(MalformedEventUTest, TextToSpeech)
{
    Firebolt::TextToSpeech::TextToSpeechImpl impl{mockHelper};
    const nlohmann::json bad = nlohmann::json::object();
    const nlohmann::json good = {{"speechid", 1}};
    expectMalformedDropped("TextToSpeech.onWillSpeak", bad, good,
                           [&](auto cb) { return impl.subscribeOnWillSpeak(std::move(cb)); });
    expectMalformedDropped("TextToSpeech.onSpeechStart", bad, good,
                           [&](auto cb) { return impl.subscribeOnSpeechStart(std::move(cb)); });
    expectMalformedDropped("TextToSpeech.onSpeechPause", bad, good,
                           [&](auto cb) { return impl.subscribeOnSpeechPause(std::move(cb)); });
    expectMalformedDropped("TextToSpeech.onSpeechResume", bad, good,
                           [&](auto cb) { return impl.subscribeOnSpeechResume(std::move(cb)); });
    expectMalformedDropped("TextToSpeech.onSpeechComplete", bad, good,
                           [&](auto cb) { return impl.subscribeOnSpeechComplete(std::move(cb)); });
    expectMalformedDropped("TextToSpeech.onSpeechInterrupted", bad, good,
                           [&](auto cb) { return impl.subscribeOnSpeechInterrupted(std::move(cb)); });
    expectMalformedDropped("TextToSpeech.onNetworkError", bad, good,
                           [&](auto cb) { return impl.subscribeOnNetworkError(std::move(cb)); });
    expectMalformedDropped("TextToSpeech.onPlaybackError", bad, good,
                           [&](auto cb) { return impl.subscribeOnPlaybackError(std::move(cb)); });
}

TEST_F(MalformedEventUTest, VideoOutput)
{
    Firebolt::VideoOutput::VideoOutputImpl impl{mockHelper};
    expectMalformedDropped("VideoOutput.onResolutionChanged", nlohmann::json{{"width", 1}},
                           nlohmann::json{{"width", 3840}, {"height", 2160}},
                           [&](auto cb) { return impl.subscribeOnResolutionChanged(std::move(cb)); });
    expectMalformedDropped("VideoOutput.onHdcpChanged", "bogus", "none",
                           [&](auto cb) { return impl.subscribeOnHdcpChanged(std::move(cb)); });
    expectMalformedDropped("VideoOutput.onCecStateChanged", "bogus", "active",
                           [&](auto cb) { return impl.subscribeOnCecStateChanged(std::move(cb)); });
    expectMalformedDropped("VideoOutput.onRefreshRateChanged", "61", 60,
                           [&](auto cb) { return impl.subscribeOnRefreshRateChanged(std::move(cb)); });
}
