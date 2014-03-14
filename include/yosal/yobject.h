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

#ifndef _YOSAL_YOBJECT_H
#define _YOSAL_YOBJECT_H 1

#ifdef __cplusplus
extern "C" {
#endif

#define YOSAL_OBJECT_STRUCTNAME(x) x##Struct
#define YOSAL_OBJECT_DECLARE(x) struct YOSAL_OBJECT_STRUCTNAME(x) {
#define YOSAL_OBJECT_BEGIN yobject _yobject;
#define YOSAL_OBJECT_END };
#define YOSAL_OBJECT_EXPORT(x) typedef struct YOSAL_OBJECT_STRUCTNAME(x) x;

/**
 * @defgroup yobject
 *
 * @brief A yobject is a C struct with helper functions to implement locking and reference
 * counting.
 *
 * Custom datatypes can be derived from yobject.
 *
 * @{
 */

/**
 * A release callback of this type has to be provided when creating a yobject. This
 * function will be responsible for releasing all memory owned by the object
 * except the yobject itself.
 *
 * @see yobject_create
 */
typedef void (*releasefunc)(void*);

struct yobjectstruct {
  void *lock; // pthread_mutex_t, but don't want to expose pthread in public struct
  releasefunc release;
  uint32_t refcount;
};
typedef struct yobjectstruct yobject;

/**
 * Allocate a yobject and initialize it.
 *
 * @param size of the object
 * @param release function
 *
 * @return newly created yobject
 */
yobject*
yobject_create(size_t size, releasefunc release);

/**
 * Retain a yobject, has to be called by everybody who holds a reference to a
 * shared object and is relying on the yobject being present. If retain is being
 * called a corresponding call to release has to be made to release the reference
 * and thus prevent a memory leak.
 *
 * @param ptr yobject to retain
 *
 * @return reference to the retained object (ptr)
 */
yobject*
yobject_retain(yobject *ptr);

/**
 * Release a yobject after it has been retained. If
 * @see yobject_retain
 *
 * @param ptr yobject to release
 *
 */
int
yobject_release(yobject *ptr);

/**
 * Lock a yobject. This function blocks until the lock can be obtained. A yobject
 * can not be locked twice, i.e. calling yobject_lock twice from the same thread
 * without releasing it between both calls will deadlock.
 *
 * @param ptr yobject to be locked
 *
 * @return YOSAL_OK on success
 */
int
yobject_lock(yobject *ptr);

/**
 * Unlock a yobject. This will fail if the calling thread does not hold the lock
 * of the object.
 *
 * @param ptr yobject to be unlocked
 *
 * @return YOSAL_OK on success
 */
int
yobject_unlock(yobject *ptr);


#ifdef __cplusplus
}
#endif

#endif /* _YOSAL_YOBJECT_H */
