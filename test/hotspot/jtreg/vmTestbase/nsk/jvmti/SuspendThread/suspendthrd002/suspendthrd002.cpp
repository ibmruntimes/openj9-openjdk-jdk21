/*
 * Copyright (c) 2003, 2024, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.
 *
 * This code is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * version 2 for more details (a copy is included in the LICENSE file that
 * accompanied this code).
 *
 * You should have received a copy of the GNU General Public License version
 * 2 along with this work; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Please contact Oracle, 500 Oracle Parkway, Redwood Shores, CA 94065 USA
 * or visit www.oracle.com if you need additional information or have any
 * questions.
 */

#include <string.h>
#include "jvmti.h"
#include "agent_common.hpp"
#include "jni_tools.h"
#include "jvmti_tools.h"

extern "C" {

/* ============================================================================= */

/* scaffold objects */
static jlong timeout = 0;

/* This is how long we verify that the thread has really suspended (in ms) */
static jlong verificationTime = 5 * 1000;

/* constant names */
#define THREAD_NAME     "TestedThread"

/* constants */
#define EVENTS_COUNT    1

/* events list */
static jvmtiEvent eventsList[EVENTS_COUNT] = {
    JVMTI_EVENT_THREAD_END
};

static volatile int eventsReceived = 0;
static jthread testedThread = nullptr;

/* ============================================================================= */

/** Agent algorithm. */
static void JNICALL
agentProc(jvmtiEnv* jvmti, JNIEnv* jni, void* arg) {

    NSK_DISPLAY0("Wait for thread to start\n");
    if (!nsk_jvmti_waitForSync(timeout))
        return;

    /* perform testing */
    {
        NSK_DISPLAY1("Find thread: %s\n", THREAD_NAME);
        if (!NSK_VERIFY((testedThread =
                nsk_jvmti_threadByName(THREAD_NAME)) != nullptr))
            return;
        NSK_DISPLAY1("  ... found thread: %p\n", (void*)testedThread);

        eventsReceived = 0;
        NSK_DISPLAY1("Enable event: %s\n", "THREAD_END");
        if (!nsk_jvmti_enableEvents(JVMTI_ENABLE, EVENTS_COUNT, eventsList, nullptr))
            return;

        NSK_DISPLAY1("Suspend thread: %p\n", (void*)testedThread);
        if (!NSK_JVMTI_VERIFY(jvmti->SuspendThread(testedThread))) {
            nsk_jvmti_setFailStatus();
            return;
        }

        NSK_DISPLAY0("Let thread to run and finish\n");
        if (!nsk_jvmti_resumeSync())
            return;

        NSK_DISPLAY1("Check that THREAD_END event NOT received for timeout: %ld ms\n", (long)verificationTime);
        {
            jlong delta = 1000;
            jlong time;
            for (time = 0; time < verificationTime; time += delta) {
                if (eventsReceived > 0) {
                    NSK_COMPLAIN0("Thread ran and finished after suspension\n");
                    nsk_jvmti_setFailStatus();
                    break;
                }
                nsk_jvmti_sleep(delta);
            }
        }

        NSK_DISPLAY1("Disable event: %s\n", "THREAD_END");
        if (!nsk_jvmti_enableEvents(JVMTI_DISABLE, EVENTS_COUNT, eventsList, nullptr))
            return;

        NSK_DISPLAY1("Resume thread: %p\n", (void*)testedThread);
        if (!NSK_JVMTI_VERIFY(jvmti->ResumeThread(testedThread))) {
            nsk_jvmti_setFailStatus();
        }

        NSK_DISPLAY0("Wait for thread to finish\n");
        if (!nsk_jvmti_waitForSync(timeout))
            return;

        NSK_DISPLAY0("Delete thread reference\n");
        NSK_TRACE(jni->DeleteGlobalRef(testedThread));
    }

    NSK_DISPLAY0("Let debugee to finish\n");
    if (!nsk_jvmti_resumeSync())
        return;
}

/* ============================================================================= */

/** THREAD_END callback. */
JNIEXPORT void JNICALL
callbackThreadEnd(jvmtiEnv* jvmti, JNIEnv* jni, jthread thread) {
    /* check if event is for tested thread */
    if (thread != nullptr &&
                jni->IsSameObject(testedThread, thread)) {
        NSK_DISPLAY1("  ... received THREAD_END event for tested thread: %p\n", (void*)thread);
        eventsReceived++;
    } else {
        NSK_DISPLAY1("  ... received THREAD_END event for unknown thread: %p\n", (void*)thread);
    }
}

/* ============================================================================= */

/** Agent library initialization. */
#ifdef STATIC_BUILD
JNIEXPORT jint JNICALL Agent_OnLoad_suspendthrd002(JavaVM *jvm, char *options, void *reserved) {
    return Agent_Initialize(jvm, options, reserved);
}
JNIEXPORT jint JNICALL Agent_OnAttach_suspendthrd002(JavaVM *jvm, char *options, void *reserved) {
    return Agent_Initialize(jvm, options, reserved);
}
JNIEXPORT jint JNI_OnLoad_suspendthrd002(JavaVM *jvm, char *options, void *reserved) {
    return JNI_VERSION_1_8;
}
#endif
jint Agent_Initialize(JavaVM *jvm, char *options, void *reserved) {
    jvmtiEnv* jvmti = nullptr;

    /* init framework and parse options */
    if (!NSK_VERIFY(nsk_jvmti_parseOptions(options)))
        return JNI_ERR;

    timeout = nsk_jvmti_getWaitTime() * 60 * 1000;

    /* create JVMTI environment */
    if (!NSK_VERIFY((jvmti =
            nsk_jvmti_createJVMTIEnv(jvm, reserved)) != nullptr))
        return JNI_ERR;

    /* add specific capabilities for suspending thread */
    {
        jvmtiCapabilities suspendCaps;
        memset(&suspendCaps, 0, sizeof(suspendCaps));
        suspendCaps.can_suspend = 1;
        if (!NSK_JVMTI_VERIFY(jvmti->AddCapabilities(&suspendCaps)))
            return JNI_ERR;
    }

    /* set callbacks for THREAD_END event */
    {
        jvmtiEventCallbacks callbacks;
        memset(&callbacks, 0, sizeof(callbacks));
        callbacks.ThreadEnd = callbackThreadEnd;
        if (!NSK_JVMTI_VERIFY(jvmti->SetEventCallbacks(&callbacks, sizeof(callbacks))))
        return JNI_ERR;
    }

    /* register agent proc and arg */
    if (!NSK_VERIFY(nsk_jvmti_setAgentProc(agentProc, nullptr)))
        return JNI_ERR;

    return JNI_OK;
}

/* ============================================================================= */

}
