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

#include <stdio.h>
#include <string.h>
#include "jvmti.h"
#include "agent_common.hpp"
#include "JVMTITools.h"

extern "C" {


#define PASSED 0
#define STATUS_FAILED 2

static jvmtiEnv *jvmti = nullptr;
static jvmtiCapabilities caps;
static jint result = PASSED;
static jboolean printdump = JNI_FALSE;

#ifdef STATIC_BUILD
JNIEXPORT jint JNICALL Agent_OnLoad_setlocal002(JavaVM *jvm, char *options, void *reserved) {
    return Agent_Initialize(jvm, options, reserved);
}
JNIEXPORT jint JNICALL Agent_OnAttach_setlocal002(JavaVM *jvm, char *options, void *reserved) {
    return Agent_Initialize(jvm, options, reserved);
}
JNIEXPORT jint JNI_OnLoad_setlocal002(JavaVM *jvm, char *options, void *reserved) {
    return JNI_VERSION_1_8;
}
#endif
jint Agent_Initialize(JavaVM *jvm, char *options, void *reserved) {
    jint res;
    jvmtiError err;

    if (options != nullptr && strcmp(options, "printdump") == 0) {
        printdump = JNI_TRUE;
    }

    res = jvm->GetEnv((void **) &jvmti, JVMTI_VERSION_1_1);
    if (res != JNI_OK || jvmti == nullptr) {
        printf("Wrong result of a valid call to GetEnv!\n");
        return JNI_ERR;
    }

    err = jvmti->GetPotentialCapabilities(&caps);
    if (err != JVMTI_ERROR_NONE) {
        printf("(GetPotentialCapabilities) unexpected error: %s (%d)\n",
               TranslateError(err), err);
        return JNI_ERR;
    }

    err = jvmti->AddCapabilities(&caps);
    if (err != JVMTI_ERROR_NONE) {
        printf("(AddCapabilities) unexpected error: %s (%d)\n",
               TranslateError(err), err);
        return JNI_ERR;
    }

    err = jvmti->GetCapabilities(&caps);
    if (err != JVMTI_ERROR_NONE) {
        printf("(GetCapabilities) unexpected error: %s (%d)\n",
               TranslateError(err), err);
        return JNI_ERR;
    }

    if (!caps.can_access_local_variables) {
        printf("Warning: access to local variables is not implemented\n");
    }

    return JNI_OK;
}

JNIEXPORT jint JNICALL
Java_nsk_jvmti_SetLocalVariable_setlocal002_check(JNIEnv *env, jclass cls, jthread thr) {
    jvmtiError err;
    jmethodID mid;
    jint entryCount;
    jvmtiLocalVariableEntry *table;
    int i;

    if (jvmti == nullptr) {
        printf("JVMTI client was not properly loaded!\n");
        return STATUS_FAILED;
    }

    if (!caps.can_access_local_variables) {
        return result;
    }

    mid = env->GetStaticMethodID(cls, "run", "([Ljava/lang/String;Ljava/io/PrintStream;)I");
    if (mid == nullptr) {
        printf("Cannot find method \"run\"\n");
        return STATUS_FAILED;
    }

    err = jvmti->GetLocalVariableTable(mid, &entryCount, &table);
    if (err != JVMTI_ERROR_NONE) {
        printf("(GetLocalVariableTable) unexpected error: %s (%d)\n",
               TranslateError(err), err);
        return STATUS_FAILED;
    }

    if (printdump == JNI_TRUE) {
        printf(">>> invalid thread check ...\n");
    }
    for (i = 0; i < entryCount; i++) {
        if (strcmp(table[i].name, "o") == 0) {
            err = jvmti->SetLocalObject(cls, 1,
                table[i].slot, (jobject)cls);
            if (err != JVMTI_ERROR_INVALID_THREAD) {
                printf("(%s) ", table[i].name);
                printf("Error expected: JVMTI_ERROR_INVALID_THREAD,\n");
                printf("\t   actual: %s (%d)\n", TranslateError(err), err);
                result = STATUS_FAILED;
            }
        } else if (strcmp(table[i].name, "i") == 0) {
            err = jvmti->SetLocalInt(cls, 1,
                table[i].slot, (jint)0);
            if (err != JVMTI_ERROR_INVALID_THREAD) {
                printf("(%s) ", table[i].name);
                printf("Error expected: JVMTI_ERROR_INVALID_THREAD,\n");
                printf("\t   actual: %s (%d)\n", TranslateError(err), err);
                result = STATUS_FAILED;
            }
        } else if (strcmp(table[i].name, "l") == 0) {
            err = jvmti->SetLocalLong(cls, 1,
                table[i].slot, (jlong)0);
            if (err != JVMTI_ERROR_INVALID_THREAD) {
                printf("(%s) ", table[i].name);
                printf("Error expected: JVMTI_ERROR_INVALID_THREAD,\n");
                printf("\t   actual: %s (%d)\n", TranslateError(err), err);
                result = STATUS_FAILED;
            }
        } else if (strcmp(table[i].name, "f") == 0) {
            err = jvmti->SetLocalFloat(cls, 1,
                table[i].slot, (jfloat)0);
            if (err != JVMTI_ERROR_INVALID_THREAD) {
                printf("(%s) ", table[i].name);
                printf("Error expected: JVMTI_ERROR_INVALID_THREAD,\n");
                printf("\t   actual: %s (%d)\n", TranslateError(err), err);
                result = STATUS_FAILED;
            }
        } else if (strcmp(table[i].name, "d") == 0) {
            err = jvmti->SetLocalDouble(cls, 1,
                table[i].slot, (jdouble)0);
            if (err != JVMTI_ERROR_INVALID_THREAD) {
                printf("(%s) ", table[i].name);
                printf("Error expected: JVMTI_ERROR_INVALID_THREAD,\n");
                printf("\t   actual: %s (%d)\n", TranslateError(err), err);
                result = STATUS_FAILED;
            }
        }
    }

    if (printdump == JNI_TRUE) {
        printf(">>> invalid frame check ...\n");
    }
    for (i = 0; i < entryCount; i++) {
        if (strcmp(table[i].name, "o") == 0) {
            err = jvmti->SetLocalObject(thr, -1,
                table[i].slot, (jobject)cls);
            if (err != JVMTI_ERROR_ILLEGAL_ARGUMENT) {
                printf("(%s) ", table[i].name);
                printf("Error expected: JVMTI_ERROR_INVALID_FRAMEID,\n");
                printf("\t   actual: %s (%d)\n", TranslateError(err), err);
                result = STATUS_FAILED;
            }
        } else if (strcmp(table[i].name, "i") == 0) {
            err = jvmti->SetLocalInt(thr, -1,
                table[i].slot, (jint)0);
            if (err != JVMTI_ERROR_ILLEGAL_ARGUMENT) {
                printf("(%s) ", table[i].name);
                printf("Error expected: JVMTI_ERROR_INVALID_FRAMEID,\n");
                printf("\t   actual: %s (%d)\n", TranslateError(err), err);
                result = STATUS_FAILED;
            }
        } else if (strcmp(table[i].name, "l") == 0) {
            err = jvmti->SetLocalLong(thr, -1,
                table[i].slot, (jlong)0);
            if (err != JVMTI_ERROR_ILLEGAL_ARGUMENT) {
                printf("(%s) ", table[i].name);
                printf("Error expected: JVMTI_ERROR_INVALID_FRAMEID,\n");
                printf("\t   actual: %s (%d)\n", TranslateError(err), err);
                result = STATUS_FAILED;
            }
        } else if (strcmp(table[i].name, "f") == 0) {
            err = jvmti->SetLocalFloat(thr, -1,
                table[i].slot, (jfloat)0);
            if (err != JVMTI_ERROR_ILLEGAL_ARGUMENT) {
                printf("(%s) ", table[i].name);
                printf("Error expected: JVMTI_ERROR_INVALID_FRAMEID,\n");
                printf("\t   actual: %s (%d)\n", TranslateError(err), err);
                result = STATUS_FAILED;
            }
        } else if (strcmp(table[i].name, "d") == 0) {
            err = jvmti->SetLocalDouble(thr, -1,
                table[i].slot, (jdouble)0);
            if (err != JVMTI_ERROR_ILLEGAL_ARGUMENT) {
                printf("(%s) ", table[i].name);
                printf("Error expected: JVMTI_ERROR_INVALID_FRAMEID,\n");
                printf("\t   actual: %s (%d)\n", TranslateError(err), err);
                result = STATUS_FAILED;
            }
        }
    }

    if (printdump == JNI_TRUE) {
        printf(">>> opague frame check ...\n");
    }
    for (i = 0; i < entryCount; i++) {
        if (strcmp(table[i].name, "o") == 0) {
            err = jvmti->SetLocalObject(thr, 0,
                table[i].slot, (jobject)cls);
            if (err != JVMTI_ERROR_OPAQUE_FRAME) {
                printf("(%s) ", table[i].name);
                printf("Error expected: JVMTI_ERROR_OPAQUE_FRAME,\n");
                printf("\t   actual: %s (%d)\n", TranslateError(err), err);
                result = STATUS_FAILED;
            }
        } else if (strcmp(table[i].name, "i") == 0) {
            err = jvmti->SetLocalInt(thr, 0,
                table[i].slot, (jint)0);
            if (err != JVMTI_ERROR_OPAQUE_FRAME) {
                printf("(%s) ", table[i].name);
                printf("Error expected: JVMTI_ERROR_OPAQUE_FRAME,\n");
                printf("\t   actual: %s (%d)\n", TranslateError(err), err);
                result = STATUS_FAILED;
            }
        } else if (strcmp(table[i].name, "l") == 0) {
            err = jvmti->SetLocalLong(thr, 0,
                table[i].slot, (jlong)0);
            if (err != JVMTI_ERROR_OPAQUE_FRAME) {
                printf("(%s) ", table[i].name);
                printf("Error expected: JVMTI_ERROR_OPAQUE_FRAME,\n");
                printf("\t   actual: %s (%d)\n", TranslateError(err), err);
                result = STATUS_FAILED;
            }
        } else if (strcmp(table[i].name, "f") == 0) {
            err = jvmti->SetLocalFloat(thr, 0,
                table[i].slot, (jfloat)0);
            if (err != JVMTI_ERROR_OPAQUE_FRAME) {
                printf("(%s) ", table[i].name);
                printf("Error expected: JVMTI_ERROR_OPAQUE_FRAME,\n");
                printf("\t   actual: %s (%d)\n", TranslateError(err), err);
                result = STATUS_FAILED;
            }
        } else if (strcmp(table[i].name, "d") == 0) {
            err = jvmti->SetLocalDouble(thr, 0,
                table[i].slot, (jdouble)0);
            if (err != JVMTI_ERROR_OPAQUE_FRAME) {
                printf("(%s) ", table[i].name);
                printf("Error expected: JVMTI_ERROR_OPAQUE_FRAME,\n");
                printf("\t   actual: %s (%d)\n", TranslateError(err), err);
                result = STATUS_FAILED;
            }
        }
    }

    if (printdump == JNI_TRUE) {
        printf(">>> ... done\n");
    }

    return result;
}

}
