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

#pragma once

#include "accessibility_impl.h"
#include "actions_impl.h"
#include "advertising_impl.h"
#include "device_impl.h"
#include "discovery_impl.h"
#include "display_impl.h"
#include "firebolt/client_version.h"
#include "firebolt/firebolt.h"
#include "lifecycle_impl.h"
#include "localization_impl.h"
#include "metrics_impl.h"
#include "network_impl.h"
#include "presentation_impl.h"
#include "speechsynthesis_impl.h"
#include "stats_impl.h"
#include "texttospeech_impl.h"
#include "videooutput_impl.h"
#include <firebolt/gateway.h>

namespace Firebolt
{
class FireboltAccessorImpl : public IFireboltAccessor
{
public:
    FireboltAccessorImpl(Firebolt::Transport::IGateway& gateway, Firebolt::Helpers::IHelper& helper)
        : gateway_(gateway),
          accessibility_(helper),
          advertising_(helper),
          actions_(helper),
          device_(helper),
          discovery_(helper),
          display_(helper),
          lifecycle_(helper),
          localization_(helper),
          metrics_(helper),
          network_(helper),
          presentation_(helper),
          speechSynthesis_(helper),
          stats_(helper),
          textToSpeech_(helper),
          videooutput_(helper)
    {
    }

    FireboltAccessorImpl(const FireboltAccessorImpl&) = delete;
    FireboltAccessorImpl& operator=(const FireboltAccessorImpl&) = delete;

    ~FireboltAccessorImpl() override { unsubscribeAll(); }

    Firebolt::Error Connect(const Firebolt::Config& config, OnConnectionChanged listener) override
    {
        gateway_.setResumeListener([this](std::chrono::milliseconds /*stoppedFor*/) { lifecycle_.reconcile(); });
        auto result = gateway_.connect(config, listener);
        FIREBOLT_LOG_NOTICE("Client", "Version: %s", Version::String);
        return result;
    }

    Firebolt::Error Disconnect() override
    {
        gateway_.setResumeListener(nullptr);
        unsubscribeAll();
        return gateway_.disconnect();
    }

    Accessibility::IAccessibility& AccessibilityInterface() override { return accessibility_; }
    Advertising::IAdvertising& AdvertisingInterface() override { return advertising_; }
    Device::IDevice& DeviceInterface() override { return device_; }
    Discovery::IDiscovery& DiscoveryInterface() override { return discovery_; }
    Display::IDisplay& DisplayInterface() override { return display_; }
    Lifecycle::ILifecycle& LifecycleInterface() override { return lifecycle_; }
    Localization::ILocalization& LocalizationInterface() override { return localization_; }
    Metrics::IMetrics& MetricsInterface() override { return metrics_; }
    Network::INetwork& NetworkInterface() override { return network_; }
    Presentation::IPresentation& PresentationInterface() override { return presentation_; }
    SpeechSynthesis::ISpeechSynthesis& SpeechSynthesisInterface() override { return speechSynthesis_; }
    Stats::IStats& StatsInterface() override { return stats_; }
    TextToSpeech::ITextToSpeech& TextToSpeechInterface() override { return textToSpeech_; }
    Actions::IActions& ActionsInterface() override { return actions_; }
    VideoOutput::IVideoOutput& VideoOutputInterface() override { return videooutput_; }

private:
    void unsubscribeAll()
    {
        accessibility_.unsubscribeAll();
        actions_.unsubscribeAll();
        device_.unsubscribeAll();
        lifecycle_.unsubscribeAll();
        localization_.unsubscribeAll();
        network_.unsubscribeAll();
        presentation_.unsubscribeAll();
        textToSpeech_.unsubscribeAll();
        videooutput_.unsubscribeAll();
        speechSynthesis_.unsubscribeAll();
    }

    Firebolt::Transport::IGateway& gateway_;
    Accessibility::AccessibilityImpl accessibility_;
    Advertising::AdvertisingImpl advertising_;
    Actions::ActionsImpl actions_;
    Device::DeviceImpl device_;
    Discovery::DiscoveryImpl discovery_;
    Display::DisplayImpl display_;
    Lifecycle::LifecycleImpl lifecycle_;
    Localization::LocalizationImpl localization_;
    Metrics::MetricsImpl metrics_;
    Network::NetworkImpl network_;
    Presentation::PresentationImpl presentation_;
    SpeechSynthesis::SpeechSynthesisImpl speechSynthesis_;
    Stats::StatsImpl stats_;
    TextToSpeech::TextToSpeechImpl textToSpeech_;
    VideoOutput::VideoOutputImpl videooutput_;
};
} // namespace Firebolt
