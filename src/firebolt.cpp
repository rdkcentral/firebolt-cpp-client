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

#include "firebolt/firebolt.h"
#include "accessibility_impl.h"
#include "actions_impl.h"
#include "advertising_impl.h"
#include "device_impl.h"
#include "discovery_impl.h"
#include "display_impl.h"
#include "firebolt/client_version.h"
#include "lifecycle_impl.h"
#include "localization_impl.h"
#include "metrics_impl.h"
#include "network_impl.h"
#include "presentation_impl.h"
#include "stats_impl.h"
#include "texttospeech_impl.h"
#include <mutex>
#include <firebolt/gateway.h>
#include <firebolt/logger.h>

namespace Firebolt
{
class FireboltAccessorImpl : public IFireboltAccessor
{
public:
    FireboltAccessorImpl()
        : accessibility_(Firebolt::Helpers::GetHelperInstance()),
          advertising_(Firebolt::Helpers::GetHelperInstance()),
          actions_(Firebolt::Helpers::GetHelperInstance()),
          device_(Firebolt::Helpers::GetHelperInstance()),
          discovery_(Firebolt::Helpers::GetHelperInstance()),
          display_(Firebolt::Helpers::GetHelperInstance()),
          lifecycle_(Firebolt::Helpers::GetHelperInstance()),
          localization_(Firebolt::Helpers::GetHelperInstance()),
          metrics_(Firebolt::Helpers::GetHelperInstance()),
          network_(Firebolt::Helpers::GetHelperInstance()),
          presentation_(Firebolt::Helpers::GetHelperInstance()),
          stats_(Firebolt::Helpers::GetHelperInstance()),
          textToSpeech_(Firebolt::Helpers::GetHelperInstance())
    {
    }

    FireboltAccessorImpl(const FireboltAccessorImpl&) = delete;
    FireboltAccessorImpl& operator=(const FireboltAccessorImpl&) = delete;

    ~FireboltAccessorImpl() { unsubscribeAll(); }

    Firebolt::Error Connect(const Firebolt::Config& config, OnConnectionChanged listener) override
    {
        Firebolt::Config effectiveConfig = config;
        {
            std::lock_guard<std::mutex> lock(logSettingsMutex_);
            if (logSettingsOverride_.has_value())
            {
                effectiveConfig.log = logSettingsOverride_.value();
            }
        }

        auto result = Firebolt::Transport::GetGatewayInstance().connect(effectiveConfig, listener);
        FIREBOLT_LOG_NOTICE("Client", "%s", Version::Banner);
        return result;
    }

    Firebolt::Error Disconnect() override
    {
        unsubscribeAll();
        return Firebolt::Transport::GetGatewayInstance().disconnect();
    }

    void SetLogSettings(const Firebolt::Config::LogSettings& settings) override
    {
        {
            std::lock_guard<std::mutex> lock(logSettingsMutex_);
            logSettingsOverride_ = settings;
        }

        // Apply logger level/format immediately for client-side logs before connect/reconnect.
        Firebolt::Logger::setLogLevel(settings.level);
        Firebolt::Logger::setFormat(settings.format.ts, settings.format.location, settings.format.function,
                                    settings.format.thread);
    }

    void ClearLogSettings() override
    {
        std::lock_guard<std::mutex> lock(logSettingsMutex_);
        logSettingsOverride_.reset();
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
    Stats::IStats& StatsInterface() override { return stats_; }
    TextToSpeech::ITextToSpeech& TextToSpeechInterface() override { return textToSpeech_; }
    Actions::IActions& ActionsInterface() override { return actions_; }

private:
    void unsubscribeAll()
    {
        accessibility_.unsubscribeAll();
        actions_.unsubscribeAll();
        lifecycle_.unsubscribeAll();
        localization_.unsubscribeAll();
        network_.unsubscribeAll();
        presentation_.unsubscribeAll();
        textToSpeech_.unsubscribeAll();
    }

private:
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
    Stats::StatsImpl stats_;
    TextToSpeech::TextToSpeechImpl textToSpeech_;
    std::mutex logSettingsMutex_;
    std::optional<Firebolt::Config::LogSettings> logSettingsOverride_;
};

/* static */ IFireboltAccessor& IFireboltAccessor::Instance()
{
    static FireboltAccessorImpl impl;
    return impl;
}
} // namespace Firebolt
