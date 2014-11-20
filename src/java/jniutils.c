/**
 * Copyright 2013-2014 Yahoo! Inc.
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

/*
 * Check if string is valid modified UTF-8 that NewStringUTF accepts.
 * This method return 0 if string is valid.
 * Otherwise returns no of 16 bit units that need to be allocated for
 * UTF-16 conversion.
 */
static int
validateUtf8(const char *utf8)
{
  unsigned char *singleByte = NULL;
  int noOfUnits = 0;
  YBOOL notValid = YFALSE;

  if (utf8 == NULL) {
    return 0;
  }

  singleByte = (unsigned char*)utf8;
  while (*singleByte != '\0') {
    if ((*singleByte & 0x80) == 0) {
      singleByte++;
      noOfUnits++;
    } else if ((*singleByte & 0x20) == 0) {
      singleByte += 2;
      noOfUnits++;
    } else if ((*singleByte & 0x10) == 0) {
      singleByte += 3;
      noOfUnits++;
    } else if ((*singleByte & 0x8) == 0) {
      singleByte += 4;
      /* For 4 byte UTF-8 we need UTF-16 pair */
      noOfUnits += 2;
      notValid = YTRUE;
    } else {
      ALOGE("Not a valid UTF-8 string %s\n", utf8);
      return 0;
    }
  }

  if (notValid == YTRUE) {
    return noOfUnits;
  } else {
    return 0;
  }
}

/*
 * Convert UTF-8 to UTF-16 to be used in NewString.
 */
static int
utf16FromUtf8(const char* utf8, jchar *utf16, int noOfUnits)
{
  unsigned char *singleByte = NULL;
  int utf16Index = 0;

  if (utf8 == NULL) {
    return utf16Index;
  }

  singleByte = (unsigned char*)utf8;
  while (*singleByte != '\0') {
    int nextBytes = 0;
    int byteIndex = 0;
    unsigned long codePoint = 0;

    if ((*singleByte & 0x80) == 0) {
      codePoint = *singleByte;
      nextBytes = 0;
    } else if ((*singleByte & 0x20) == 0) {
      codePoint = *singleByte & 0x1F;
      nextBytes = 1;
    } else if ((*singleByte & 0x10) == 0) {
      codePoint = *singleByte & 0x0F;
      nextBytes = 2;
    } else if ((*singleByte & 0x8) == 0) {
      codePoint = *singleByte & 0x07;
      nextBytes = 3;
    } else {
      ALOGE("Not a valid UTF-8 character %c\n", *singleByte);
      return utf16Index;
    }

    singleByte++;
    for (byteIndex = 0; byteIndex < nextBytes; ++byteIndex) {
      if (*singleByte == '\0') {
        ALOGE("Not a valid UTF-8 string %s\n", utf8);
        return utf16Index;
      }
      if ((*singleByte & 0xC0) != 0x80) {
        /* subsequent bytes should be always 10xxxxxx */
        ALOGE("Not a valid UTF-8 string %s\n", utf8);
        return utf16Index;
      }
      codePoint <<= 6;
      codePoint += *singleByte & 0x3F;
      singleByte++;
    }

    if (utf16Index < noOfUnits) {
      if (codePoint < 0x10000) {
        utf16[utf16Index] = codePoint;
        utf16Index++;
      } else if (codePoint <= 0x10FFFF) {
        /* Code points U+010000 to U+10FFFF need surrogate pairs */
        utf16[utf16Index] = ((codePoint >> 10) + 0xD7C0);
        utf16Index++;
        utf16[utf16Index] = ((codePoint & 0x3FF) + 0xDC00);
        utf16Index++;
      } else {
        ALOGE("Not a valid UTF-8 character %s\n", utf8);
        return utf16Index;
      }
    } else {
      ALOGE("Falied to convert %s to UTF-16\n", utf8);
      return utf16Index;
    }
  }

  return utf16Index;
}

jstring
jniutils_NewStringUTF(JNIEnv* _env, const char *data)
{
  jstring result = NULL;

  if (data != NULL) {
    int noOfUnits = validateUtf8(data);

    /* if noOfUnits is 0, it is valid Modified UTF-8. */
    if (noOfUnits == 0) {
      result = (*_env)->NewStringUTF(_env, data);
    } else {
      /* Convert to UTF-16. */
      jchar *buffer = (jchar *)Ymem_calloc(noOfUnits, sizeof(jchar));
      if (buffer != NULL) {
        int success = utf16FromUtf8(data, buffer, noOfUnits);
        if (success == noOfUnits) {
          result = (*_env)->NewString(_env, buffer, noOfUnits);
        }
        Ymem_free(buffer);
      }
    }

    if ((*_env)->ExceptionCheck(_env)) {
      (*_env)->ExceptionDescribe(_env);
      (*_env)->ExceptionClear(_env);
    }
  }

  return result;
}
