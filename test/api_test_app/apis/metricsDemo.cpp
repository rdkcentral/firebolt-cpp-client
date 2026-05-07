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

#include "metricsDemo.h"
#include <firebolt/firebolt.h>
#include <iostream>
#include <string>

using namespace Firebolt;
using namespace Firebolt::Metrics;

MetricsDemo::MetricsDemo()
    : DemoBase("Metrics")
{
    methods_.push_back("Metrics.ready");
    methods_.push_back("Metrics.signIn");
    methods_.push_back("Metrics.signOut");
    methods_.push_back("Metrics.startContent");
    methods_.push_back("Metrics.stopContent");
    methods_.push_back("Metrics.page");
    methods_.push_back("Metrics.error");
    methods_.push_back("Metrics.mediaLoadStart");
    methods_.push_back("Metrics.mediaPlay");
    methods_.push_back("Metrics.mediaPlaying");
    methods_.push_back("Metrics.mediaPause");
    methods_.push_back("Metrics.mediaWaiting");
    methods_.push_back("Metrics.mediaSeeking");
    methods_.push_back("Metrics.mediaSeeked");
    methods_.push_back("Metrics.mediaRateChanged");
    methods_.push_back("Metrics.mediaRenditionChanged");
    methods_.push_back("Metrics.mediaEnded");
    methods_.push_back("Metrics.event");
    methods_.push_back("Metrics.appInfo");
}

void MetricsDemo::runOption(const std::string& method)
{
    std::cout << "Running Metrics method: " << method << std::endl;
    if (method == "Metrics.ready")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().ready();
        if (succeed(r))
        {
            std::cout << "Metrics Ready: Success" << std::endl;
        }
    }
    else if (method == "Metrics.signIn")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().signIn();
        if (succeed(r))
        {
            std::cout << "Metrics Sign In: Success" << std::endl;
        }
    }
    else if (method == "Metrics.signOut")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().signOut();
        if (succeed(r))
        {
            std::cout << "Metrics Sign Out: Success" << std::endl;
        }
    }
    else if (method == "Metrics.startContent")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().startContent("entity123", std::nullopt);
        if (succeed(r))
        {
            std::cout << "Metrics Start Content: Success" << std::endl;
        }
    }
    else if (method == "Metrics.stopContent")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().stopContent("entity123", std::nullopt);
        if (succeed(r))
        {
            std::cout << "Metrics Stop Content: Success" << std::endl;
        }
    }
    else if (method == "Metrics.page")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().page("page123", std::nullopt);
        if (succeed(r))
        {
            std::cout << "Metrics Page: Success" << std::endl;
        }
    }
    else if (method == "Metrics.error")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().error(ErrorType::Media, "ERR001",
                                                                                  "An error occurred", true,
                                                                                  std::nullopt, std::nullopt);
        if (succeed(r))
        {
            std::cout << "Metrics Error: Success" << std::endl;
        }
    }
    else if (method == "Metrics.mediaLoadStart")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaLoadStart("entity123", std::nullopt);
        if (succeed(r))
        {
            std::cout << "Metrics Media Load Start: Success" << std::endl;
        }
    }
    else if (method == "Metrics.mediaPlay")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaPlay("entity123", std::nullopt);
        if (succeed(r))
        {
            std::cout << "Metrics Media Play: Success" << std::endl;
        }
    }
    else if (method == "Metrics.mediaPlaying")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaPlaying("entity123", std::nullopt);
        if (succeed(r))
        {
            std::cout << "Metrics Media Playing: Success" << std::endl;
        }
    }
    else if (method == "Metrics.mediaPause")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaPause("entity123", std::nullopt);
        if (succeed(r))
        {
            std::cout << "Metrics Media Pause: Success" << std::endl;
        }
    }
    else if (method == "Metrics.mediaWaiting")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaWaiting("entity123", std::nullopt);
        if (succeed(r))
        {
            std::cout << "Metrics Media Waiting: Success" << std::endl;
        }
    }
    else if (method == "Metrics.mediaSeeking")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaSeeking("entity123", 0.5, std::nullopt);
        if (succeed(r))
        {
            std::cout << "Metrics Media Seeking: Success" << std::endl;
        }
    }
    else if (method == "Metrics.mediaSeeked")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaSeeked("entity123", 0.5, std::nullopt);
        if (succeed(r))
        {
            std::cout << "Metrics Media Seeked: Success" << std::endl;
        }
    }
    else if (method == "Metrics.mediaRateChanged")
    {
        auto r =
            Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaRateChanged("entity123", 1.5, std::nullopt);
        if (succeed(r))
        {
            std::cout << "Metrics Media Rate Changed: Success" << std::endl;
        }
    }
    else if (method == "Metrics.mediaRenditionChanged")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaRenditionChanged("entity123", 3000,
                                                                                                  1920, 1080, "HDR",
                                                                                                  std::nullopt);
        if (succeed(r))
        {
            std::cout << "Metrics Media Rendition Changed: Success" << std::endl;
        }
    }
    else if (method == "Metrics.mediaEnded")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().mediaEnded("entity123", std::nullopt);
        if (succeed(r))
        {
            std::cout << "Metrics Media Ended: Success" << std::endl;
        }
    }
    else if (method == "Metrics.event")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().event("https://com.example.event",
                                                                                  "{\"key\":\"value\"}", std::nullopt);
        if (succeed(r))
        {
            std::cout << "Metrics Event: Success" << std::endl;
        }
    }
    else if (method == "Metrics.appInfo")
    {
        auto r = Firebolt::IFireboltAccessor::Instance().MetricsInterface().appInfo("build123");
        if (succeed(r))
        {
            std::cout << "Metrics App Info: " << std::boolalpha << "Success" << std::endl;
        }
    }
}
