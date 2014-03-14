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

#ifndef _YOSAL_QUEUE_H
#define _YOSAL_QUEUE_H 1

#ifdef __cplusplus
extern "C" {
#endif

/* Implementation-dependant queue type */
typedef struct YqueueStruct Yqueue;
typedef struct YqueueRecordStruct YqueueRecord;

/**
 * @defgroup Yqueue
 *
 * @brief Portable queue implementation
 *
 * @{
 */

/**
 * Create a new Yqueue instance. All Yqueues have to be released using
 * Yqueue_release when they are no longer needed.
 *
 * @return newly created Yqueue
 */
Yqueue*
Yqueue_create();

/**
 * Release an existing Yqueue.
 *
 * @param l queue to be released
 *
 * @return YOSAL_OK on success
 */
int
Yqueue_release(Yqueue *l);

/**
 * Insert a new value at the end of a Yqueue.
 *
 * @param l queue to append to
 * @param value to be inserted
 *
 * @return new queue size
 */
int
Yqueue_push(Yqueue *l, void *value);

/**
 * Insert a new value at the beginning of a Yqueue.
 *
 * @param l queue to prepend to
 * @param value to be inserted
 *
 * @return new queue size
 */
int
Yqueue_insert(Yqueue *l, void *value);

/**
 * Retrieve and remove the first value from a Yqueue.
 *
 * @param l queue to pop from
 *
 * @return retrieved value
 */
void*
Yqueue_pop(Yqueue *l);

/**
 * Obtain a reference to the first value of a Yqueue.
 *
 * @param l queue to obtain reference from
 *
 * @return retrieved value
 */
void*
Yqueue_fetch(Yqueue *l);

/**
 * Determine the size of a Yqueue.
 *
 * @param l queue whose size is needed
 *
 * @return size of l
 */
int
Yqueue_size(Yqueue *l);

/**
 * Begin iterating over a Yqueue
 *
 * @param l queue to iterate on
 *
 * @return first record in l
 */
YqueueRecord*
Yqueue_first(Yqueue *l);

/**
 * Continue iterating over a Yqueue
 *
 * @param l queue to iterate on
 * @param element current record
 *
 * @return next record in l
 */
YqueueRecord*
Yqueue_next(Yqueue *l, YqueueRecord *element);

/**
 * Obtain a reference to the value of a YqueueRecord
 *
 * @param element the record
 *
 * @return value of element
 */
void*
Yqueue_value(const YqueueRecord *element);

/**
 * Obtain a sorted array from a populated Yqueue.
 *
 * @param l yqueue to be sorted
 * @param compar comparison function to compare two queue records.
 * @see http://www.gnu.org/software/libc/manual/html_node/Array-Sort-Function.html
 */
void**
Yqueue_array(Yqueue *l, int(*compar)(const void *, const void *));

#ifdef __cplusplus
}
#endif

#endif /* _YOSAL_QUEUE_H */
