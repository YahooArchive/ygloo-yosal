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
 * @file   ybuffer.h
 * @brief  Dynamic buffer
 */

#ifndef _YOSAL_YBUFFER_H
#define _YOSAL_YBUFFER_H 1

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Ybuffer
 *
 * @brief This module provides an implementation of a dynamic memory buffer
 *
 * @{
 */
typedef struct YbufferStruct Ybuffer;

Ybuffer*
Ybuffer_init(int initiallength);

/**
 * This function returns a pointer to the data held by a Ybuffer and
 * destroys the encapsulating Ybuffer.
 *
 * @param stream YBuffer whose data is to be freed
 * @param[out] datalen Length of the data held by the Ybuffer
 *
 * @return data held by the buffer
 */
char* Ybuffer_detach(Ybuffer *stream, int *datalen);

/**
 * Clear all data contained by the Ybuffer after it is no longer needed. After
 * After calling this function it is safe to free the Ybuffer using Ymem_free.
 *
 * @param stream buffer to be cleaned
 */
void
Ybuffer_fini(Ybuffer *stream);

/**
 * Append string buf to Ybuffer stream. buflen can either be the length of
 * buf or -1 if the length of string is not known to the caller.
 *
 * @param stream Ybuffer to append data to
 * @param buf data which will be appended
 * @param buflen length of buf
 *
 * @return length of appended buffer or -1 on failure
 */
int
Ybuffer_append(Ybuffer *stream, const char *buf, int buflen);

/**
 * Append an integer to a buffer. The integer is converted to a string before
 * appending it.
 *
 * @param stream Ybuffer to append integer to
 * @param value integer to be appended
 *
 * @return length of appended buffer or -1 on failure
 */
int
Ybuffer_append_integer(Ybuffer *stream, int value);

/**
 * @}
 */
#ifdef __cplusplus
};
#endif

#endif /* _YOSAL_BUFFER_H */
