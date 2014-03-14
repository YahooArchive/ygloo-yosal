/**
 * Copyright 2013 Yahoo! Inc.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may
 * obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License. See accompanying LICENSE file.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define LOG_TAG "yosal::jni"
#include "yosal/yosal.h"

/*
 * Register several native methods for one class.
 */
int jniutils_registerNativeMethods(JNIEnv* _env, const char* className,
				    JNINativeMethod* gMethods, int numMethods)
{
  jclass clazz;

  clazz = (*_env)->FindClass(_env, className);
  if (clazz == NULL) {
    ALOGE("Native registration unable to find class '%s'", className);
    return JNI_FALSE;
  }
  if ((*_env)->RegisterNatives(_env, clazz, gMethods, numMethods) < 0) {
    ALOGE("RegisterNatives failed for '%s'", className);
    return JNI_FALSE;
  }

  ALOGD("Registered native API for '%s'", className);
  return JNI_TRUE;
}

/*
 * Register several native methods for one class.
 */
int jniutils_findFields(JNIEnv *_env, struct field *fields, int count)
{
  int i;

  for (i = 0; i < count; i++) {
    struct field *f = &fields[i];
    jclass clazz = (*_env)->FindClass(_env, f->class_name);
    if (clazz == NULL) {
      ALOGE("Can't find %s", f->class_name);
      return -1;
    }

    jfieldID field = (*_env)->GetFieldID(_env, clazz, f->field_name, f->field_type);
    if (field == NULL) {
      ALOGE("Can't find %s.%s", f->class_name, f->field_name);
      return -1;
    }

    *(f->jfield) = field;
  }

  return 0;
}
