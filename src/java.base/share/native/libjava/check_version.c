/*
 * Copyright (c) 1998, 2021, Oracle and/or its affiliates. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * This code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 only, as
 * published by the Free Software Foundation.  Oracle designates this
 * particular file as subject to the "Classpath" exception as provided
 * by Oracle in the LICENSE file that accompanied this code.
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

/*
 * ===========================================================================
 * (c) Copyright IBM Corp. 2025, 2025 All Rights Reserved
 * ===========================================================================
 */

#include "jni.h"
#include "jni_util.h"
#include "jvm.h"

#if !defined(WIN32)
#include "j9access.h"
/* tracehelp.c defines getTraceInterfaceFromVM(), used by J9_UTINTERFACE_FROM_VM(). */
#include "tracehelp.c"
#include "ut_jcl_java.c"
#endif /* !defined(WIN32) */

JNIEXPORT jint JNICALL
DEF_JNI_OnLoad(JavaVM *vm, void *reserved)
{
#if !defined(WIN32)
    /* Windows doesn't call JNI_OnLoad for libjava.dll, initialize the tracepoint elsewhere. */
    UT_JCL_JAVA_MODULE_LOADED(J9_UTINTERFACE_FROM_VM((J9JavaVM *)vm));
#endif /* !defined(WIN32) */

    return JNI_VERSION_1_2;
}
