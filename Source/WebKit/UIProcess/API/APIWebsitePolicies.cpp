/*
 * Copyright (C) 2017 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "APIWebsitePolicies.h"

#include "WebsiteDataStore.h"
#include "WebsitePoliciesData.h"

namespace API {

WebsitePolicies::WebsitePolicies() = default;

WebsitePolicies::WebsitePolicies(bool contentBlockersEnabled, OptionSet<WebKit::WebsiteAutoplayQuirk> allowedAutoplayQuirks, WebKit::WebsiteAutoplayPolicy autoplayPolicy, Vector<WebCore::HTTPHeaderField>&& legacyCustomHeaderFields, Vector<WebCore::CustomHeaderFields>&& customHeaderFields, WebKit::WebsitePopUpPolicy popUpPolicy, RefPtr<WebKit::WebsiteDataStore>&& websiteDataStore)
    : m_contentBlockersEnabled(contentBlockersEnabled)
    , m_allowedAutoplayQuirks(allowedAutoplayQuirks)
    , m_autoplayPolicy(autoplayPolicy)
    , m_legacyCustomHeaderFields(WTFMove(legacyCustomHeaderFields))
    , m_customHeaderFields(WTFMove(customHeaderFields))
    , m_popUpPolicy(popUpPolicy)
    , m_websiteDataStore(WTFMove(websiteDataStore))
{ }

Ref<WebsitePolicies> WebsitePolicies::copy() const
{
    auto policies = WebsitePolicies::create();
    policies->setContentBlockersEnabled(m_contentBlockersEnabled);
    policies->setAllowedAutoplayQuirks(m_allowedAutoplayQuirks);
    policies->setAutoplayPolicy(m_autoplayPolicy);
#if ENABLE(DEVICE_ORIENTATION)
    policies->setDeviceOrientationAndMotionAccessState(m_deviceOrientationAndMotionAccessState);
#endif
    policies->setPopUpPolicy(m_popUpPolicy);
    policies->setWebsiteDataStore(m_websiteDataStore.get());
    policies->setCustomUserAgent(m_customUserAgent);
    policies->setCustomJavaScriptUserAgentAsSiteSpecificQuirks(m_customJavaScriptUserAgentAsSiteSpecificQuirks);
    policies->setCustomNavigatorPlatform(m_customNavigatorPlatform);
    policies->setPreferredContentMode(m_preferredContentMode);
    policies->setMetaViewportPolicy(m_metaViewportPolicy);
    policies->setMediaSourcePolicy(m_mediaSourcePolicy);
    policies->setSimulatedMouseEventsDispatchPolicy(m_simulatedMouseEventsDispatchPolicy);
    policies->setLegacyOverflowScrollingTouchPolicy(m_legacyOverflowScrollingTouchPolicy);
    policies->setAllowContentChangeObserverQuirk(m_allowContentChangeObserverQuirk);
    
    Vector<WebCore::HTTPHeaderField> legacyCustomHeaderFields;
    legacyCustomHeaderFields.reserveInitialCapacity(m_legacyCustomHeaderFields.size());
    for (auto& field : m_legacyCustomHeaderFields)
        legacyCustomHeaderFields.uncheckedAppend(field);
    policies->setLegacyCustomHeaderFields(WTFMove(legacyCustomHeaderFields));

    Vector<WebCore::CustomHeaderFields> customHeaderFields;
    customHeaderFields.reserveInitialCapacity(m_customHeaderFields.size());
    for (auto& field : m_customHeaderFields)
        customHeaderFields.uncheckedAppend(field);
    policies->setCustomHeaderFields(WTFMove(customHeaderFields));
    policies->setAllowSiteSpecificQuirksToOverrideContentMode(m_allowSiteSpecificQuirksToOverrideContentMode);
    policies->setApplicationNameForDesktopUserAgent(m_applicationNameForDesktopUserAgent);
    return policies;
}

WebsitePolicies::~WebsitePolicies()
{
}

void WebsitePolicies::setWebsiteDataStore(RefPtr<WebKit::WebsiteDataStore>&& websiteDataStore)
{
    m_websiteDataStore = WTFMove(websiteDataStore);
}

WebKit::WebsitePoliciesData WebsitePolicies::data()
{
    bool hasLegacyCustomHeaderFields = legacyCustomHeaderFields().size();
    Vector<WebCore::CustomHeaderFields> customHeaderFields;
    customHeaderFields.reserveInitialCapacity(this->customHeaderFields().size() + hasLegacyCustomHeaderFields);
    for (auto& field : this->customHeaderFields())
        customHeaderFields.uncheckedAppend(field);
    if (hasLegacyCustomHeaderFields)
        customHeaderFields.uncheckedAppend({ legacyCustomHeaderFields(), { }});

    return {
        contentBlockersEnabled(),
        allowedAutoplayQuirks(),
        autoplayPolicy(),
#if ENABLE(DEVICE_ORIENTATION)
        deviceOrientationAndMotionAccessState(),
#endif
        WTFMove(customHeaderFields),
        popUpPolicy(),
        m_websiteDataStore ? Optional<WebKit::WebsiteDataStoreParameters> { m_websiteDataStore->parameters() } : WTF::nullopt,
        m_customUserAgent,
        m_customJavaScriptUserAgentAsSiteSpecificQuirks,
        m_customNavigatorPlatform,
        m_metaViewportPolicy,
        m_mediaSourcePolicy,
        m_simulatedMouseEventsDispatchPolicy,
        m_legacyOverflowScrollingTouchPolicy,
        m_allowContentChangeObserverQuirk,
    };
}

}

