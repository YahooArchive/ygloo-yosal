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
 * @file   yalloc.h
 * @addtogroup Yalloc
 * @brief  Custom memory allocation
 */
#ifndef _YOSAL_YALLOC_H
#define _YOSAL_YALLOC_H 1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Yalloc Yalloc
 *
 * This module provides API for memory allocation and management
 *
 * @{
 */

/**
 * @brief Allocate memory in heap
 * @ingroup yosal
 *
 * Allocate memory in heap.
 *
 * @param size number of bytes to allocate
 * @return pointer to allocated memory, or NULL on error
 */
void *
Ymem_malloc(size_t size);

/**
 * @brief Allocate memory in heap
 * @ingroup yosal
 *
 * Tries to change the size of the allocation pointed to by ptr to size, and returns ptr.
 * If there is not enough room to enlarge the memory allocation pointed to by ptr,
 * Ymem_realloc() creates a new allocation, copies as much of the old data pointed
 * to by ptr as will fit to the new allocation, frees the old allocation, and
 * returns a pointer to the allocated memory.  If ptr is NULL, Ymem_realloc()
 * is identical to a call to Ymem_malloc() for size bytes.
 *
 * @param ptr pointer to previously allocated memory region
 * @param size number of bytes to allocate
 * @return pointer to allocated memory, or NULL on error
 */
void *
Ymem_realloc(void *ptr, size_t size);

/**
 * @brief Free memory in heap
 * @ingroup yosal
 *
 * Release memory allocated with Ymem_malloc, Ymem_realloc, Ymem_calloc or Ymem_strdup.
 *
 * @param ptr pointer to previously allocated memory region
 * @noreturn pointer to allocated memory, or NULL on error
 */
void
Ymem_free(void *ptr);

/**
 * @brief Copy null terminated string
 * @ingroup yosal
 *
 * Allocates sufficient memory for a copy of a string, does the copy, and returns
 * a pointer to it.  The pointer may subsequently be released with Ymem_free.
 *
 * @param str null terminated string to copy
 * @return pointer to copy of string, or NULL on error
 */
char *
Ymem_strdup(const char *str);

/**
 * @brief Allocate continuous memory region for objects
 * @ingroup yosal
 *
 * Contiguously allocates enough space for count objects that are size bytes of
 * memory each and returns a pointer to the allocated memory.  The allocated memory
 * is filled with bytes of value zero.
 *
 * @param nmemb number of objects
 * @param size size of each object
 * @return pointer to allocated  memory region, or NULL on error
 */
void *
Ymem_calloc(size_t nmemb, size_t size);

/**
 * @brief Check if a pointer is aligned
 * @ingroup yosal
 *
 * Check if pointer is aligned
 *
 * @param ptr pointer to check
 * @param alignment alignment constraint
 * @retval 1 if pointer is aligned
 * @retval 0 otherwise
 */
int Ymem_isaligned (void *ptr, size_t alignment);

/**
 * @brief Allocate aligned memory in heap
 * @ingroup yosal
 *
 * Allocate memory in heap, returning a reference to the allocated region
 * and an aligned pointer, with the guarantee that the memory region referenced
 * by the aligned pointer is at least size bytes
 *
 * @param alignment requested alignment
 * @param size number of bytes to allocate
 * @param alignedref reference to the aligned pointer to allocated memory, or NULL on error

 * @return pointer to allocated memory, or NULL on error
 */
void *
Ymem_malloc_aligned(size_t alignment, size_t size, void **alignedref);

#ifdef __cplusplus
};
#endif

#endif /* _YOSAL_YALLOC_H */
