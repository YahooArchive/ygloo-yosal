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

struct YArrayStruct {
  int length;
  int maxlength;
  int (*releaseElement) (void*);
  void **elements;
};


YArray*
YArray_create()
{
  return YArray_createLength(8);
}

YArray*
YArray_createLength(int initlength)
{
  YArray *array;

  array = Ymem_malloc(sizeof(YArray));
  if (array == NULL) {
    return NULL;
  }

  if (initlength <= 0) {
    array->elements = NULL;
  } else {
    array->elements = Ymem_malloc(initlength * sizeof(void*));
    if (array->elements == NULL) {
      initlength = 0;
    }
  }

  array->length = 0;
  array->maxlength = initlength;
  array->releaseElement = NULL;

  return array;
}

int
YArray_reset(YArray *array)
{
  if (array == NULL) {
    return YOSAL_ERROR;
  }

  if (array->elements != NULL) {
    if (array->releaseElement != NULL) {
      int i;
      for (i = 0; i < array->length; i++) {
        if (array->elements[i] != NULL) {
          (*array->releaseElement)(array->elements[i]);
        }
      }
    }
  }

  array->length = 0;
  return YOSAL_OK;
}

int
YArray_append(YArray *array, void *element)
{
  if (array == NULL) {
    return YOSAL_OK;
  }
  if (element == NULL) {
    return YOSAL_OK;
  }

  if (array->length >= array->maxlength) {
    int newlength;
    void **elements;

    if (array->maxlength < 64) {
      newlength = array->maxlength + 64;
    } else {
      newlength = array->maxlength + (array->maxlength / 4);
    }

    elements = (void**) Ymem_realloc(array->elements, newlength*sizeof(void*));
    if (elements == NULL) {
      return YOSAL_ERROR;
    }

    array->elements = elements;
    array->maxlength = newlength;
  }

  array->elements[array->length] = element;
  array->length++;

  return YOSAL_OK;
}

int
YArray_release(YArray *array)
{
  if (array == NULL) {
    return YOSAL_OK;
  }

  YArray_reset(array);

  if (array->elements != NULL) {
    Ymem_free(array->elements);
  }

  Ymem_free(array);

  return YOSAL_OK;
}

void*
YArray_get(YArray *array, int n)
{
  if ((array == NULL) || (n < 0) || (n >= array->length)) {
    return NULL;
  }

  return array->elements[n];
}

void*
YArray_detach(YArray *array, int n)
{
  void *result = NULL;

  if ((array == NULL) || (n < 0) || (n >= array->length)) {
    return NULL;
  }

  result = array->elements[n];
  array->elements[n] = NULL;

  return result;
}

void*
YArray_prune(YArray *array, int n)
{
  void *result = NULL;

  if ((array == NULL) || (n < 0) || (n >= array->length)) {
    return NULL;
  }

  if (array->elements[n] != NULL) {
    result = array->elements[n];
    if (array->releaseElement != NULL) {
      array->releaseElement(result);
    }
    array->elements[n] = NULL;
  }

  return result;
}

int
YArray_length(const YArray *array)
{
  if (array == NULL) {
    return 0;
  }
  
  return array->length;
}

int
YArray_setElementReleaseFunc(YArray *array, YArrayElementReleaseFunc releaseElement)
{
  if (array == NULL) {
    return YOSAL_ERROR;
  }

  array->releaseElement = releaseElement;
  return YOSAL_OK;
}
