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

/**
 *
 * A simple queue structure in C
 *
 * Copyright 2013 Yahoo
 */

#include "yosal/yosal.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

struct YqueueRecordStruct {
  void *value;
  struct YqueueRecordStruct *next;
};

struct YqueueStruct {
  unsigned int count;
  YqueueRecord *first;
  YqueueRecord *last;
};

Yqueue*
Yqueue_create()
{
  Yqueue *l;

  l = Ymem_malloc(sizeof(Yqueue));
  if (l == NULL) {
    return NULL;
  }

  l->count = 0;
  l->first = NULL;
  l->last = NULL;

  return l;
}

int
Yqueue_release(Yqueue *l)
{
  if (l != NULL) {
    while (Yqueue_size(l) > 0) {
      Yqueue_pop(l);
    }
    Ymem_free(l);
  }

  return YOSAL_OK;
}

int
Yqueue_push(Yqueue *l, void *value)
{
  YqueueRecord *record;

  record = Ymem_malloc(sizeof(YqueueRecord));
  record->value = value;
  record->next = NULL;

  if (l->last != NULL) {
    l->last->next = record;
  }
  if (l->first == NULL) {
    l->first = record;
  }

  l->last = record;
  l->count++;

  return l->count;
}

int
Yqueue_insert(Yqueue *l, void *value)
{
  YqueueRecord *record;

  record = Ymem_malloc(sizeof(YqueueRecord));
  record->value = value;
  record->next = l->first;

  if (l->last == NULL) {
    l->last = record;
  }

  l->first = record;
  l->count++;

  return l->count;
}

void*
Yqueue_pop(Yqueue *l)
{
  YqueueRecord *record;
  void* value;

  if (l == NULL || l->first == NULL) {
    return NULL;
  }

  record = l->first;
  value = record->value;

  l->first = record->next;
  if (l->first == NULL) {
    /* This was last element, list is now empty */
    l->last = NULL;
  }
  l->count--;

  Ymem_free(record);
  return value;
}

void*
Yqueue_fetch(Yqueue *l)
{
  if (l == NULL || l->first == NULL) {
    return NULL;
  }

  return l->first->value;
}

int
Yqueue_size(Yqueue *l)
{
  if (l == NULL) {
    return 0;
  }

  return l->count;
}

/* Iterator for queue */
YqueueRecord*
Yqueue_first(Yqueue *l)
{
  if (l == NULL) {
    return NULL;
  }
  return l->first;
}

YqueueRecord*
Yqueue_next(Yqueue *l, YqueueRecord *element)
{
  if (l == NULL || element == NULL) {
    return NULL;
  }
  return element->next;
}

void*
Yqueue_value(const YqueueRecord *element)
{
  return (element == NULL ? NULL : element->value);
}

void**
Yqueue_array(Yqueue *l, int(*compar)(const void *, const void *))
{
  void **qarray;
  YqueueRecord *qrec;
  int qlen;
  int i;

  if (l == NULL) {
    return NULL;
  }

  qlen = Yqueue_size(l);
  if (qlen <= 0) {
    return NULL;
  }

  qarray = Ymem_malloc(qlen * sizeof(void*));

  i = 0;
  qrec = Yqueue_first(l);
  while (qrec != NULL) {
    qarray[i++] = Yqueue_value(qrec);
    qrec = Yqueue_next(l, qrec);
  }

  if (compar != NULL && qlen > 1) {
    qsort(qarray, qlen, sizeof(void*), compar);
  }

  return qarray;
}
