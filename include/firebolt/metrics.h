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

#pragma once

#include "firebolt/common_types.h"
#include <firebolt/types.h>
#include <map>
#include <string>

namespace Firebolt::Metrics
{

enum class ErrorType
{
    Network,
    Media,
    Restriction,
    Entitlement,
    Other
};

class IMetrics
{
public:
    virtual ~IMetrics() = default;

    /**
     * @brief Informs the platform that the app is minimally usable
     *
     * @retval The success state or error
     */
    virtual Result<bool> ready() const = 0;

    /**
     * @brief Logs a sign in event
     *
     * @retval The success state or error
     */
    virtual Result<bool> signIn() const = 0;

    /**
     * @brief Logs a sign out event
     *
     * @retval The success state or error
     */
    virtual Result<bool> signOut() const = 0;

    /**
     * @brief Informs the platform that your user has started content
     *
     * @param[in] entityId  : The ID of the content entity
     * @param[in] agePolicy : The age policy to associate with the metrics event, the age policy describes the age group
     *                        to which content is directed
     *
     * @retval The success state or error
     */
    virtual Result<bool> startContent(const std::optional<std::string>& entityId,
                                      const std::optional<Firebolt::AgePolicy> agePolicy) const = 0;

    /**
     * @brief Informs the platform that your user has stopped content
     *
     * @param[in] entityId  : The ID of the content entity
     * @param[in] agePolicy : The age policy to associate with the metrics event, the age policy describes the age group
     *                        to which content is directed
     *
     * @retval The success state or error
     */
    virtual Result<bool> stopContent(const std::optional<std::string>& entityId,
                                     const std::optional<Firebolt::AgePolicy> agePolicy) const = 0;

    /**
     * @brief Informs the platform that your user has navigated to a page or view
     *
     * @param[in] pageId    : The ID of the content entity
     * @param[in] agePolicy : The age policy to associate with the metrics event, the age policy describes the age group
     *                        to which content is directed
     *
     * @retval The success state or error
     */
    virtual Result<bool> page(const std::string& pageId, const std::optional<Firebolt::AgePolicy>& agePolicy) const = 0;

    /**
     * @brief Informs the platform of an error that has occurred in your app
     *
     * @param[in] type        : The type of error
     * @param[in] code        : An app-specific error code
     * @param[in] description : The error description
     * @param[in] visible     : Whether the error is visible to the user
     * @param[in] parameters  : Additional parameters for the error as key-value pairs
     * @param[in] agePolicy   : The age policy to associate with the metrics event, the age policy describes the age
     *                          group to which content is directed
     *
     * @retval The success state or error
     */
    virtual Result<bool> error(const ErrorType type, const std::string& code, const std::string& description,
                               const bool visible, const std::optional<std::map<std::string, std::string>>& parameters,
                               const std::optional<Firebolt::AgePolicy>& agePolicy) const = 0;

    /**
     * @brief Called when setting the URL of a media asset to play, in order to infer load time
     *
     * @param[in] entityId  : The ID of the media entity
     * @param[in] agePolicy : The age policy to associate with the metrics event, the age policy describes the age group
     *                        to which content is directed
     *
     * @retval The success state or error
     */
    virtual Result<bool> mediaLoadStart(const std::string& entityId,
                                        const std::optional<Firebolt::AgePolicy>& agePolicy) const = 0;

    /**
     * @brief Called when media playback actually starts due to autoplay, user-initiated play, unpausing, or recovering
     *        from a buffering interruption
     *
     * @param[in] entityId  : The ID of the media entity
     * @param[in] agePolicy : The age policy to associate with the metrics event, the age policy describes the age group
     *                        to which content is directed
     *
     * @retval The success state or error
     */
    virtual Result<bool> mediaPlaying(const std::string& entityId,
                                      const std::optional<Firebolt::AgePolicy>& agePolicy) const = 0;

    /**
     * @brief Called when media playback should start due to autoplay, user-initiated play, or unpausing
     *
     * @param[in] entityId  : The ID of the media entity
     * @param[in] agePolicy : The age policy to associate with the metrics event, the age policy describes the age group
     *                        to which content is directed
     *
     * @retval The success state or error
     */
    virtual Result<bool> mediaPlay(const std::string& entityId,
                                   const std::optional<Firebolt::AgePolicy>& agePolicy) const = 0;

    /**
     * @brief Called when media playback will pause due to an intentional pause operation
     *
     * @param[in] entityId  : The ID of the media entity
     * @param[in] agePolicy : The age policy to associate with the metrics event, the age policy describes the age group
     *                        to which content is directed
     *
     * @retval The success state or error
     */
    virtual Result<bool> mediaPause(const std::string& entityId,
                                    const std::optional<Firebolt::AgePolicy>& agePolicy) const = 0;

    /**
     * @brief Called when media playback will halt due to a network, buffer, or other unintentional constraint
     *
     * @param[in] entityId  : The ID of the media entity
     * @param[in] agePolicy : The age policy to associate with the metrics event, the age policy describes the age group
     *                        to which content is directed
     *
     * @retval The success state or error
     */
    virtual Result<bool> mediaWaiting(const std::string& entityId,
                                      const std::optional<Firebolt::AgePolicy>& agePolicy) const = 0;

    /**
     * @brief Called when a seek is initiated during media playback
     *
     * @param[in] entityId  : The ID of the media entity
     * @param[in] target    : Target destination of the seek, as a decimal percentage (0-0.999) for content with a known
     *                        duration, or an integer number of seconds (0-86400) for content with an unknown duration
     * @param[in] agePolicy : The age policy to associate with the metrics event, the age policy describes the age group
     *                        to which content is directed
     *
     * @retval The success state or error
     */
    virtual Result<bool> mediaSeeking(const std::string& entityId, const double target,
                                      const std::optional<Firebolt::AgePolicy>& agePolicy) const = 0;

    /**
     * @brief Called when a seek is completed during media playback
     *
     * @param[in] entityId  : The ID of the media entity
     * @param[in] position  : Resulting position of the seek operation, as a decimal percentage (0-0.999) for content
     *                        with a known duration, or an integer number of seconds (0-86400) for content with an
     *                        unknown duration
     * @param[in] agePolicy : The age policy to associate with the metrics event, the age policy describes the age group
     *                        to which content is directed
     *
     * @retval The success state or error
     */
    virtual Result<bool> mediaSeeked(const std::string& entityId, const double position,
                                     const std::optional<Firebolt::AgePolicy>& agePolicy) const = 0;

    /**
     * @brief Called when the playback rate of media is changed
     *
     * @param[in] entityId  : The ID of the media entity
     * @param[in] rate      : The new playback rate
     * @param[in] agePolicy : The age policy to associate with the metrics event, the age policy describes the age group
     *                        to which content is directed
     *
     * @retval The success state or error
     */
    virtual Result<bool> mediaRateChanged(const std::string& entityId, const double rate,
                                          const std::optional<Firebolt::AgePolicy>& agePolicy) const = 0;

    /**
     * @brief Called when the rendition of media is changed, such as bitrate, dimensions, or profile
     *
     * @param[in] entityId  : The ID of the media entity
     * @param[in] bitrate   : The new bitrate in kbps
     * @param[in] width     : The new resolution width
     * @param[in] height    : The new resolution height
     * @param[in] profile   : A description of the new profile, e.g. 'HDR' etc.
     * @param[in] agePolicy : The age policy to associate with the metrics event, the age policy describes the age group
     *                        to which content is directed
     *
     * @retval The success state or error
     */
    virtual Result<bool> mediaRenditionChanged(const std::string& entityId, const unsigned bitrate, const unsigned width,
                                               const unsigned height, const std::optional<std::string>& profile,
                                               const std::optional<Firebolt::AgePolicy>& agePolicy) const = 0;

    /**
     * @brief Called when playback has stopped because the end of the media was reached
     *
     * @param[in] entityId  : The ID of the media entity
     * @param[in] agePolicy : The age policy to associate with the metrics event, the age policy describes the age group
     *                        to which content is directed
     *
     * @retval The success state or error
     */
    virtual Result<bool> mediaEnded(const std::string& entityId,
                                    const std::optional<Firebolt::AgePolicy>& agePolicy) const = 0;

    /**
     * @brief Called to inform the platform of 1st party distributor metrics
     *
     * @param[in] schema    : The schema URI of the metric type
     * @param[in] data      : A JSON payload
     * @param[in] agePolicy : The age policy to associate with the metrics event, the age policy describes the age group
     *                        to which content is directed
     *
     * @retval The success state or error
     */
    virtual Result<bool> event(const std::string& schema, const std::string& data,
                               const std::optional<Firebolt::AgePolicy>& agePolicy) const = 0;

    /**
     * @brief Inform the platform about an app's build info
     *
     * @param[in] build : The build / version of this app
     *
     * @retval The status
     */
    virtual Result<void> appInfo(const std::string& build) const = 0;
};

} // namespace Firebolt::Metrics
