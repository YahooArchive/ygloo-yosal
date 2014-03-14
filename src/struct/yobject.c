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

#define LOG_TAG "yosal::yobject"

#include "yosal/yosal.h"
#include "pthread.h"

#if !defined(NDEBUG)
// Disable debug logging.
#undef ALOGD
#define ALOGD(...) ((void)0)
#endif


yobject*
yobject_create(size_t size, releasefunc release)
{
  yobject *yptr = Ymem_malloc(size);
  if (yptr == NULL) {
    return NULL;
  }

  yptr->lock = Ymem_malloc(sizeof(pthread_mutex_t));
  if (yptr->lock == NULL) {
    Ymem_free(yptr);
    return NULL;
  }

  if (pthread_mutex_init(yptr->lock, NULL) != 0) {
    Ymem_free(yptr->lock);
    Ymem_free(yptr);
    return NULL;
  };

  yptr->refcount = 0;
  yptr->release = release;
  
  ALOGD("create %p with lock %p", yptr, yptr->lock);

  return yptr;
}

yobject*
yobject_retain(yobject *yptr)
{
  if (yptr == NULL) {
    return NULL;
  }
  
  ALOGD("retain %p", yptr);

  if (yobject_lock(yptr) != YOSAL_OK) {
    return NULL;
  }

  yptr->refcount++;
  yobject_unlock(yptr);

  return yptr;
}

int
yobject_release(yobject *yptr)
{
  if (yptr == NULL) {
    return YOSAL_OK;
  }
  
  ALOGD("release %p", yptr);

  pthread_mutex_t *mymutex = yptr->lock;

  if (yobject_lock(yptr) != YOSAL_OK) {
    return YOSAL_ERROR;
  }

  if (yptr->refcount > 1) {
    int myrefcount;
    yptr->refcount = yptr->refcount-1;
    myrefcount = yptr->refcount;
    pthread_mutex_unlock(mymutex);
    return myrefcount;
  }

  yptr->release(yptr);
  yptr = NULL;

  if (mymutex != NULL) {
    pthread_mutex_unlock(mymutex);
    pthread_mutex_destroy(mymutex);
    Ymem_free(mymutex);
  }

  return 0;
}


int
yobject_lock(yobject *yptr)
{
  if (yptr != NULL && yptr->lock != NULL) {
    if (pthread_mutex_lock(yptr->lock) != 0) {
      ALOGE("failed to lock yobject");
      return YOSAL_ERROR;
    }
  }

  return YOSAL_OK;
}

int
yobject_unlock(yobject *yptr)
{
  if (yptr != NULL && yptr->lock != NULL) {
    if (pthread_mutex_unlock(yptr->lock) != 0) {
      ALOGE("failed to unlock yobject");
      return YOSAL_ERROR;
    }
  }
  
  return YOSAL_OK;
}
