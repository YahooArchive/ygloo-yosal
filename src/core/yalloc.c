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
#include <string.h>
#include <limits.h>
#ifndef WIN32
#  include <unistd.h>
#endif

#include "yosal/yosal.h"

void *Ymem_malloc(size_t size)
{
  if (size == 0) {
    return NULL;
  }

  return malloc(size);
}

void *
Ymem_realloc(void *ptr, size_t size)
{  
  if (size <= 0) {
    return NULL;
  }

  return realloc(ptr, size);
}

void Ymem_free(void *ptr)
{
  if (ptr != NULL) {
    free(ptr);
  }
}

char *
Ymem_strdup(const char *str)
{
  char *result = NULL;
  size_t l;

  if (str != NULL) {
    l = strlen(str);
    result = (char*) Ymem_malloc(l+1);
    if (result != NULL) {
      memcpy(result, str, l+1);
    }
  }

  return result;
}


void *
Ymem_calloc(size_t nmemb, size_t size)
{
  size_t l;
  void *result = NULL;

  /* check for overflow */
  if (size > ((size_t)-1) / nmemb) {
    return NULL;
  }

  l = nmemb * size;
  if (l < nmemb || l < size) {
    return NULL;
  }

  result = Ymem_malloc(l);
  if (result != NULL) {
    memset(result, 0, l);
  }

  return result;
}

static size_t
PointerOffset(void *ptr, size_t alignment) {
    if (ptr == NULL || alignment <= 1) {
        return 0;
    }

    return (size_t) ( ((unsigned long) ptr) & (alignment - 1) );
}

static void *
PointerAlign(void *ptr, size_t alignment)
{
    size_t n = PointerOffset(ptr, alignment);
    if (n == 0) {
        return ptr;
    }

    return (void*) ( ((char*) ptr) + (alignment - n) );
}

int Ymem_isaligned (void *ptr, size_t alignment) {
    return (PointerOffset(ptr, alignment) == 0);
}

void *
Ymem_malloc_aligned(size_t alignment, size_t size, void **alignedref)
{
    void *ptr = NULL;
    void *alignedptr = NULL;

    if (size <= 0 || alignment <= 1) {
        ptr = Ymem_malloc(size);
        alignedptr = ptr;
    } else {
#if 0
        /* Libc allocator supports memory alignment (i.e. memalign() or posix_memalign()) */
#if defined(__BIONIC__)
        ptr = memalign(alignment, size);
#else
        if (posix_memalign(&ptr, alignment, size) != 0) {
            ptr = NULL;
        }
#endif
        alignedptr = ptr;
#else
        /* Emulate aligned memory allocation using standard malloc() */
        ptr = Ymem_malloc(size);
        if (ptr != NULL && Ymem_isaligned(ptr, alignment)) {
            alignedptr = ptr;
        } else {
            ptr = Ymem_realloc(ptr, size + alignment);
            alignedptr = PointerAlign(ptr, alignment);
        }
#endif
    }

    if (alignedref != NULL) {
        *alignedref = alignedptr;
    }

    return ptr;
}
