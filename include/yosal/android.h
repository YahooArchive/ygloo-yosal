/**
 * Stubs and replacements for AOSP provided functions that will be used on
 * non-AOSP platforms.
 */

/*
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _YOSAL_ANDROID_H
#define _YOSAL_ANDROID_H 1

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

#if defined(ANDROID) || defined(__ANDROID)
#define YOSAL_CONFIG_ANDROID_JNI 1
#include <jni.h>
#include <android/log.h>
#else /* ANDROID */
#define YOSAL_CONFIG_ANDROID_EMULATION 1
#include "yosal/android_jni.h"
#include "yosal/android_log.h"
#endif /* ANDROID */

/* On all platform, JNI types and definitions are supported */
#define YOSAL_CONFIG_JNI 1

/* no effect on Android */
void __android_set_loglevel(int prio);

#ifndef ALOG_SET_LOGLEVEL
#define ALOG_SET_LOGLEVEL __android_set_loglevel
#endif

#ifndef ALOG_DEBUG
#if defined(DEBUG) || !defined(NDEBUG)
#define ALOG_DEBUG 1
#else
#define ALOG_DEBUG 0
#endif
#endif

#ifndef ALOGV
#if ALOG_DEBUG
#define  ALOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG,__VA_ARGS__)
#else
#define ALOGV(...) ((void)0)
#endif
#endif

#ifndef ALOGD
#if ALOG_DEBUG
#define  ALOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define ALOGD(...) ((void)0)
#endif
#endif

#ifndef ALOGI
#define  ALOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#endif
#ifndef ALOGW
#define  ALOGW(...)  __android_log_print(ANDROID_LOG_WARNING,LOG_TAG,__VA_ARGS__)
#endif
#ifndef ALOGE
#define  ALOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif

#define NELEM(x) ((int)(sizeof(x)/sizeof(x[0])))

struct field {
    const char *class_name;
    const char *field_name;
    const char *field_type;
    jfieldID   *jfield;
};

int
jniutils_registerNativeMethods(JNIEnv* env, const char* className,
			       JNINativeMethod* gMethods, int numMethods);

int
jniutils_findFields(JNIEnv *env, struct field *fields, int count);

jstring
jniutils_NewStringUTF(JNIEnv* _env, const char *data);

#ifdef __cplusplus
};
#endif

#endif /* _YOSAL_ANDROID_H */
