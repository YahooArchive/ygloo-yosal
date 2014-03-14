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

#ifndef _YOSAL_ARRAY_H
#define	_YOSAL_ARRAY_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct YArrayStruct YArray;
typedef int (*YArrayElementReleaseFunc)(void *element);

/**
 * @defgroup YArray
 *
 * @brief Dynamically growing array
 *
 * YArray provides an array with an (optional) initial length that can grow
 * as needed. Dynamic shrinking is not supported, a YArray can only be truncated
 * to 0 length.
 *
 * @{
 */

/**
 * Creates a new YArray with a minimal amount of pre-allocated memory. This
 * YArray has to be released later using YArray_release.
 *
 * @return newly created YArray
 */
YArray*
YArray_create();

/**
 * Creates a new YArray, which pre-alloctes memory to hold initlength element
 * pointers. This YArray has to be released later using YArray_release. A YArray
 * grows automatically on demand to be able to insert new pointers beyond the
 * initial length.
 *
 * @param initlength number of elements that will be stored in this YArray
 *
 * @return newly created YArray
 */
YArray*
YArray_createLength(int initlength);

/**
 * Truncate the given YArray to 0. Release all elements held by the given YArray
 * if an element release function has been set using YArray_setElementReleaseFunc
 * previously.
 *
 * @param array to be truncated
 *
 * @return YOSAL_OK on success
 */
int
YArray_reset(YArray *array);

/**
 * Append element to the given YArray
 *
 * @param array to append element to
 * @param element to be appended
 *
 * @return YOSAL_OK on success
 */
int
YArray_append(YArray *array, void *element);

/**
 * Release the given YArray. If you wish to release the elements of the YArray,
 * call Yarray_reset before calling YArray_release.
 *
 * @param array to be released
 *
 * @return YOSAL_OK on success
 */
int
YArray_release(YArray *array);

/**
 * Retrieve reference to element at position n from array. This function returns
 * NULL if n is greater than the number of elements that were inserted
 * previously.
 *
 * @param array to obtain element from
 * @param n index of element
 *
 * @return element at position n
 */
void*
YArray_get(YArray *array, int n);

/**
 * Remove element at position n from array and return its reference. This
 * function returns NULL if n is greater than the number of elements that were
 * inserted previously.
 *
 * @param array to remove element from
 * @n index of element
 *
 * @return element that has been removed from array
 */
void*
YArray_detach(YArray *array, int n);

/**
 * Remove element at position n from array release it if an element release
 * function has been set previously using YArray_setElementReleaseFunc. If no
 * release function has been set, this function behaves like YArray_detach.
 * Returns NULL if n is greater than the number of elements that were
 * inserted previously.
 *
 * @param array to remove element from
 * @n index of element
 *
 * @return element that has been removed from array
 */
void*
YArray_prune(YArray *array, int n);

/**
 * Obtain the length of the given YArray.
 *
 * @param array whose length is needed
 *
 * @return length of the given array
 */
int
YArray_length(const YArray *array);

/**
 * Set the element release function for a YArray. This element release function
 * can be called by the YArray when functions like YArray_reset or Yarray_prune
 * are being called. You can unset a previously set release function by setting
 * the release function to NULL.
 *
 * The signature of the release function is as follows:
 * <code>int release(void* element);</code>
 *
 * @param array to set release function for
 * @param pointer of type YArrayElementReleaseFunc
 *
 * @return YOSAL_OK on success
 */
int
YArray_setElementReleaseFunc(YArray *array, YArrayElementReleaseFunc releaseElement);


#ifdef	__cplusplus
}
#endif

#endif	/* _YOSAL_ARRAY_H */

