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

#include "yosal/yosal.h"

#include <unistd.h>
#include <errno.h>

#define JARRAY_BUF_SIZE   (16*1024)

typedef struct {
  JNIEnv  *env;
  jobject stream;

#if YOSAL_CONFIG_JNI
  jbyteArray inbytearray;
  jbyteArray outbytearray;

  jclass inputStreamClass;
  jmethodID resetMethodID;
  jmethodID readMethodID;
  jclass outputStreamClass;
  jmethodID writeMethodID;
  jmethodID closeMethodID;
#endif
} YchannelJavaStream;

#if YOSAL_CONFIG_JNI
static YchannelJavaStream*
engineCreate()
{
  YchannelJavaStream *engine = NULL;

  engine = Ymem_malloc(sizeof(YchannelJavaStream));
  if (engine != NULL) {
    engine->env = NULL;
    engine->stream = NULL;
    engine->inbytearray = NULL;
    engine->outbytearray = NULL;

    engine->inputStreamClass = NULL;
    engine->resetMethodID = 0;
    engine->readMethodID = 0;
    engine->outputStreamClass = NULL;
    engine->writeMethodID = 0;
    engine->closeMethodID = 0;
  }

  return engine;
}

static int
engineRelease(YchannelJavaStream *engine)
{
  JNIEnv *_env;

  if (engine == NULL) {
    return YOSAL_OK;
  }

  _env = engine->env;
  if (_env != NULL) {
    if (engine->stream != 0) {
      (*_env)->DeleteGlobalRef(_env, engine->stream);
    }

    if (engine->inbytearray != 0) {
      (*_env)->DeleteGlobalRef(_env, engine->inbytearray);
    }
    if (engine->inputStreamClass != 0) {
      (*_env)->DeleteGlobalRef(_env, engine->inputStreamClass);
    }
    if (engine->outbytearray != 0) {
      (*_env)->DeleteGlobalRef(_env, engine->outbytearray);
    }
    if (engine->outputStreamClass != 0) {
      (*_env)->DeleteGlobalRef(_env, engine->outputStreamClass);
    }
  }

  Ymem_free(engine);

  return YOSAL_OK;
}

static int
YchannelJavaStreamRead(Ychannel *channel, void *readbuf, int nbytes)
{
  int n = -1;
  YchannelJavaStream *engine;
  jint toread;
  JNIEnv *_env;
  int ntries = 0;

  engine = (YchannelJavaStream*) YchannelGetEngine(channel);
  if (engine == NULL) {
    return -1;
  }
  if (nbytes <= 0) {
    return 0;
  }

  _env = engine->env;

  toread = (jint) nbytes;
  if (toread > JARRAY_BUF_SIZE) {
    toread = JARRAY_BUF_SIZE;
  }

  while (1) {
    n = (*_env)->CallIntMethod(_env,
                               engine->stream,
                               engine->readMethodID,
                               engine->inbytearray, 0,
                               toread);

    ntries++;

    if ((*_env)->ExceptionCheck(_env)) {
      /* read threw an exception, abort */
      /* (*env)->ExceptionDescribe(env); */
      (*_env)->ExceptionClear(_env);
      n = -1;
    } else if (n < 0) {
      /* EOF, abort (and normalize return code to -1 */
      n = -1;
    } else if (n == 0) {
      /*
       * n == 0 should not be possible, see InputStream read() specifications.
       * However, broken implementation of some specific InputStream seems to
       * sometimes do buffering in an invalid way and actually return empty buffer
       */
      if (ntries < 2) {
        continue;
      }
      /*
       * Enfore actual return code if really failed to read a single byte, even if
       * no exception was thrown
       */
      n = -1;
    } else {
      (*_env)->GetByteArrayRegion(_env, engine->inbytearray, 0, n, (jbyte*) readbuf);
      if ((*_env)->ExceptionCheck(_env)) {
        /* (*_env)->ExceptionDescribe(_env); */
        (*_env)->ExceptionClear(_env);
        n = -1;
      }
    }
    break;
  }

  return n;
}

static int
YchannelJavaStreamWrite(Ychannel *channel, const void *buf, int nbytes)
{
  YchannelJavaStream *engine;
  jint towrite;
  JNIEnv *_env;
  int written;

  engine = (YchannelJavaStream*) YchannelGetEngine(channel);
  if (engine == NULL) {
    return -1;
  }

  _env = engine->env;

  towrite = (jint) nbytes;
  if (towrite > JARRAY_BUF_SIZE) {
    towrite = JARRAY_BUF_SIZE;
  }

  (*_env)->SetByteArrayRegion(_env, engine->outbytearray,
                              0, towrite, (jbyte*) buf);
  if ((*_env)->ExceptionCheck(_env)) {
    /* (*_env)->ExceptionDescribe(_env); */
    (*_env)->ExceptionClear(_env);
    return -1;
  }

  (*_env)->CallVoidMethod(_env, engine->stream, engine->writeMethodID,
                          engine->outbytearray, 0, towrite);
  if ((*_env)->ExceptionCheck(_env)) {
    /* (*env)->ExceptionDescribe(env); */
    (*_env)->ExceptionClear(_env);
    /* write threw an exception */
    written = -1;
  } else {
    written = (int) towrite;
  }

  return written;
}

static int
YchannelJavaStreamRelease(Ychannel *channel)
{
  YchannelJavaStream *engine;
  JNIEnv *_env;

  engine = (YchannelJavaStream*) YchannelGetEngine(channel);
  if (engine == NULL) {
    return -1;
  }

  _env = engine->env;

  if (YchannelGetAutoRelease(channel)) {
    if (engine->closeMethodID != NULL) {
      (*_env)->CallVoidMethod(_env,
                              engine->stream,
                              engine->closeMethodID);
    }
  }

  engineRelease(engine);

  return 0;
}
#endif

Ychannel*
YchannelInitJavaInputStream(JNIEnv *_env, jobject inputstream)
{
  Ychannel *channel = NULL;

#if YOSAL_CONFIG_JNI
  YchannelJavaStream *engine;
  jclass inputStream_clazz = 0;
  jmethodID inputStream_resetMethodID = 0;
  jmethodID inputStream_readMethodID = 0;
  jmethodID inputStream_closeMethodID = 0;
  jbyteArray inbytearray;

  inputStream_clazz = (*_env)->GetObjectClass(_env, inputstream);
  if (inputStream_clazz == 0) {
    return NULL;
  }

  inputStream_resetMethodID = (*_env)->GetMethodID(_env, inputStream_clazz, "reset", "()V");
  inputStream_readMethodID = (*_env)->GetMethodID(_env, inputStream_clazz, "read", "([BII)I");
  inputStream_closeMethodID = (*_env)->GetMethodID(_env, inputStream_clazz, "close", "()V");

  /* Have close() method optional, so ignore if it could not be resolved */
  if (inputStream_resetMethodID == 0 || inputStream_readMethodID == 0) {
    return NULL;
  }

  inbytearray = (*_env)->NewByteArray(_env, JARRAY_BUF_SIZE);
  if (inbytearray == NULL) {
    return NULL;
  }

  engine = engineCreate();
  if (engine == NULL) {
    return NULL;
  }

  /* Guarantee that stream, buffer and class are not released */
  engine->stream = (*_env)->NewGlobalRef(_env, inputstream);
  if (engine->stream == NULL) {
    engineRelease(engine);
    return NULL;
  }

  engine->inbytearray = (*_env)->NewGlobalRef(_env, inbytearray);
  if (engine->inbytearray == NULL) {
    engineRelease(engine);
    return NULL;
  }

  engine->inputStreamClass = (*_env)->NewGlobalRef(_env, inputStream_clazz);
  if (engine->inputStreamClass == NULL) {
    engineRelease(engine);
    return NULL;
  }

  engine->env = _env;
  engine->resetMethodID = inputStream_resetMethodID;
  engine->readMethodID = inputStream_readMethodID;
  engine->closeMethodID = inputStream_closeMethodID;

#if 0
  (*_env)->CallVoidMethod(_env, engine->stream, engine->resetMethodID);
  if ((*_env)->ExceptionCheck(_env)) {
    /* (*_env)->ExceptionDescribe(_env); */
    (*_env)->ExceptionClear(_env);
  }
#endif

  channel = YchannelInitGeneric("javastream", engine,
                                YchannelJavaStreamRead, NULL,
                                NULL, YchannelJavaStreamRelease);
  if (channel == NULL) {
    engineRelease(engine);
  }
#endif

  return channel;
}

Ychannel*
YchannelInitJavaOutputStream(JNIEnv *_env, jobject outputstream)
{
  Ychannel *channel = NULL;

#if YOSAL_CONFIG_JNI
  YchannelJavaStream *engine;
  jclass outputStream_clazz = 0;
  jmethodID outputStream_writeMethodID = 0;
  jmethodID outputStream_closeMethodID = 0;
  jbyteArray outbytearray;

  outputStream_clazz = (*_env)->GetObjectClass(_env, outputstream);
  if (outputStream_clazz == NULL) {
    return NULL;
  }

  outputStream_writeMethodID =
    (*_env)->GetMethodID(_env, outputStream_clazz, "write", "([BII)V");
  outputStream_closeMethodID =
    (*_env)->GetMethodID(_env, outputStream_clazz, "close", "()V");

  /* Have close() method optional, so ignore if it could not be resolved */
  if (outputStream_writeMethodID == 0) {
    return NULL;
  }

  outbytearray = (*_env)->NewByteArray(_env, JARRAY_BUF_SIZE);
  if (outbytearray == NULL) {
    return NULL;
  }

  engine = engineCreate();
  if (engine == NULL) {
    return NULL;
  }

  /* Guarantee that stream, buffer and class are not released */
  engine->stream = (*_env)->NewGlobalRef(_env, outputstream);
  if (engine->stream == NULL) {
    engineRelease(engine);
    return NULL;
  }

  engine->outbytearray = (*_env)->NewGlobalRef(_env, outbytearray);
  if (engine->outbytearray == NULL) {
    engineRelease(engine);
    return NULL;
  }

  engine->outputStreamClass = (*_env)->NewGlobalRef(_env, outputStream_clazz);
  if (engine->outputStreamClass == NULL) {
    engineRelease(engine);
    return NULL;
  }

  engine->env = _env;
  engine->writeMethodID = outputStream_writeMethodID;
  engine->closeMethodID = outputStream_closeMethodID;

  channel = YchannelInitGeneric("javastream", engine,
                                NULL, YchannelJavaStreamWrite,
                                NULL, YchannelJavaStreamRelease);
  if (channel == NULL) {
    engineRelease(engine);
  }
#endif

  return channel;
}
