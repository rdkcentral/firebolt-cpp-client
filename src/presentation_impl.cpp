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

#include "presentation_impl.h"
#include <firebolt/json_types.h>

namespace Firebolt::Presentation
{
PresentationImpl::PresentationImpl(Firebolt::Helpers::IHelper& helper)
    : helper_(helper),
      subscriptionManager_(helper, this)
{
}

Result<bool> PresentationImpl::focused() const
{
    return helper_.get<Firebolt::JSON::Boolean, bool>("Presentation.focused");
}

Result<SubscriptionId> PresentationImpl::subscribeOnFocusedChanged(std::function<void(bool)>&& notification)
{
    return subscriptionManager_.subscribe<Firebolt::JSON::Boolean, bool>("Presentation.onFocusedChanged",
                                                                         std::move(notification));
}

Result<void> PresentationImpl::unsubscribe(SubscriptionId id)
{
    return subscriptionManager_.unsubscribe(id);
}

void PresentationImpl::unsubscribeAll()
{
    subscriptionManager_.unsubscribeAll();
}
} // namespace Firebolt::Presentation
