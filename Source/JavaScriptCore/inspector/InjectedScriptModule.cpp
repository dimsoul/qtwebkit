/*
 * Copyright (C) 2013 Apple Inc. All rights reserved.
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "InjectedScriptModule.h"

#include "InjectedScript.h"
#include "InjectedScriptManager.h"
#include "ScriptFunctionCall.h"
#include "ScriptObject.h"

namespace Inspector {

InjectedScriptModule::InjectedScriptModule(const String& name)
    : InjectedScriptBase(name)
{
}

InjectedScriptModule::~InjectedScriptModule()
{
}

void InjectedScriptModule::ensureInjected(InjectedScriptManager* injectedScriptManager, JSC::ExecState* scriptState)
{
    InjectedScript injectedScript = injectedScriptManager->injectedScriptFor(scriptState);
    ensureInjected(injectedScriptManager, injectedScript);
}

void InjectedScriptModule::ensureInjected(InjectedScriptManager* injectedScriptManager, const InjectedScript& injectedScript)
{
    ASSERT(!injectedScript.hasNoValue());
    if (injectedScript.hasNoValue())
        return;

    // FIXME: Make the InjectedScript a module itself.
    JSC::JSLockHolder locker(injectedScript.scriptState());
    Deprecated::ScriptFunctionCall function(injectedScript.injectedScriptObject(), "hasInjectedModule"_s, injectedScriptManager->inspectorEnvironment().functionCallHandler());
    function.appendArgument(name());
    auto hasInjectedModuleResult = injectedScript.callFunctionWithEvalEnabled(function);
    ASSERT(hasInjectedModuleResult);
    if (!hasInjectedModuleResult) {
        auto& error = hasInjectedModuleResult.error();
        ASSERT(error);
        unsigned line = 0;
        unsigned column = 0;
        auto& stack = error->stack();
        if (stack.size() > 0)
            stack[0].computeLineAndColumn(line, column);
        WTFLogAlways("Error when calling 'hasInjectedModule' for '%s': %s (%d:%d)\n", name().utf8().data(), error->value().toWTFString(injectedScript.scriptState()).utf8().data(), line, column);
        WTFLogAlways("%s\n", source().utf8().data());
        RELEASE_ASSERT_NOT_REACHED();
    }
    if (!hasInjectedModuleResult.value().isBoolean() || !hasInjectedModuleResult.value().asBoolean()) {
        Deprecated::ScriptFunctionCall function(injectedScript.injectedScriptObject(), "injectModule"_s, injectedScriptManager->inspectorEnvironment().functionCallHandler());
        function.appendArgument(name());
        function.appendArgument(source());
        function.appendArgument(host(injectedScriptManager, injectedScript.scriptState()));
        auto injectModuleResult = injectedScript.callFunctionWithEvalEnabled(function);
        if (!injectModuleResult) {
            auto& error = injectModuleResult.error();
            ASSERT(error);
            unsigned line = 0;
            unsigned column = 0;
            auto& stack = error->stack();
            if (stack.size() > 0)
                stack[0].computeLineAndColumn(line, column);
            WTFLogAlways("Error when calling 'injectModule' for '%s': %s (%d:%d)\n", name().utf8().data(), error->value().toWTFString(injectedScript.scriptState()).utf8().data(), line, column);
            WTFLogAlways("%s\n", source().utf8().data());
            RELEASE_ASSERT_NOT_REACHED();
        }
    }
}

} // namespace Inspector
