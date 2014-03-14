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
 * @file   ychannel.h
 * @addtogroup Yosal Ychannel
 * @brief  Abstraction for I/O channel
 */
#ifndef _YOSAL_YCHANNEL_H
#define _YOSAL_YCHANNEL_H 1

#include "yosal/yosal.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Ychannel
 *
 * @brief Stackable input/output channel that can read from or write to different types of data.
 *
 * For example a Ychannel could be backed by a file descriptor, a
 * memory buffer or basically anything that the Ychannel callbacks can be
 * implemented for. Refer to src/io/engine/ for example implementations of these
 * functions. An implementation of all 4 callbacks is called an engine and the data
 * they operate to is referred to as engine data.
 *
 * @{
 */


/** An opaque channel type */
typedef struct YchannelStruct Ychannel;

/**
 * Callback that is used to read from a Ychannel. This function needs to know
 * how to read data from this Ychannel instance. This function can read up to
 * nbytes.
 *
 * @param channel current Ychannel
 * @param readbuf buffer to read data into
 * @param nbytes number of bytes that are being requested
 *
 * @return number of bytes read, -1 on error and 0 on EOF
 */
typedef int (*YchannelReadCB)(Ychannel *channel, void *readbuf, int nbytes);

/**
 * Callback that is used to write to a Ychannel. This function needs to know
 * how to write data to this Ychannel instance.
 *
 * @param channel current Ychannel
 * @param buf buffer to be written into the Ychannel
 * @param towrite number of bytes to be written
 *
 * @return number of bytes written or -1 on error
 */
typedef int (*YchannelWriteCB)(Ychannel *channel, void *buf, int towrite);

/**
 * Callback that is used to flush a Ychannel. After this function returns
 * all writes are guaranteed to have been persisted unless this Ychannel is backed
 * by memory only.
 *
 * @param channel current Ychannel
 *
 * @return YOSAL_OK on success
 */
typedef int (*YchannelFlushCB)(Ychannel *channel);

/**
 * Callback that is called when a Ychannel is being released. This function has to
 * release all memory related to the Ychannel that does not belong to Ychannel itself
 * as well as do other cleanup, such as closing file descriptors.
 *
 * @param channel 
 *
 * @return YOSAL_OK on success
 */
typedef int (*YchannelReleaseCB)(Ychannel *channel);

/**
 * @brief Create new Ychannel from memory buffer
 * @ingroup yosal
 *
 * Allocate a Ychannel object, taking its input from an existing
 * memory region.
 *
 * @param header a memory buffer.
 * @param hlength length (in bytes) of the memory buffer
 * @return Void
 */
Ychannel*
YchannelInitByteArray(const char *header, int hlength);

/**
 * @brief Create new Ychannel from Posix FILE
 * @ingroup yosal
 *
 * Allocate a Ychannel object, taking its input from an existing
 * FILE.
 *
 * @param file an opened FILE* channel as returned by fopen()
 * @param writable If true, create an output channel
 * @return A readable Ychannel object.
 */
Ychannel*
YchannelInitFile(FILE *file, int writable);

/**
 * @brief Create new Ychannel from file descriptor
 * @ingroup yosal
 *
 * Allocate a Ychannel object, taking its input from an opened
 * file descriptor
 *
 * @param fd an opened file descriptor
 * @param writable If true, create an output channel
 * @return A readable Ychannel object.
 */
Ychannel*
YchannelInitFd(int fd, int writable);

/**
 * @brief Create new readable Ychannel from Java InputStream
 * @ingroup yosal
 *
 * Allocate a Ychannel object, taking its input from an existing
 * Java InputStream
 *
 * @param jenv Java environment
 * @param inputstream Java InputStream object
 * @return A readable Ychannel object.
 */
Ychannel*
YchannelInitJavaInputStream(JNIEnv *jenv, jobject inputstream);

/**
 * @brief Create new writable Ychannel from Java OutputStream
 * @ingroup yosal
 *
 * Allocate a Ychannel object, using an existing Java OutputStream
 * as output
 *
 * @param jenv Java environment
 * @param inputstream Java InputStream object
 * @return A readable Ychannel object.
 */
Ychannel*
YchannelInitJavaOutputStream(JNIEnv *_env, jobject outputstream);

/**
 * Automatically release underlying file descriptor, FILE
 * or ByteArray
 *
 * WARNING: Does not automatically close underlying Java streams
 * today!
 *
 * @param channel
 * @param autorelease
 * @return
 */
int
YchannelSetAutoRelease(Ychannel* channel, int autorelease);

/**
 * Retrieve auto-release state
 * @param channel
 * @return
 */
int
YchannelGetAutoRelease(Ychannel* channel);

/**
 * @brief Destructor for a Ychannel object
 * @ingroup yosal
 *
 * Close a Ychannel object, releasing all its internal resources
 *
 * @param channel Ychannel object to release
 * @retval YMAGINE_OK   Success
 * @retval YMAGINE_ERROR  Failure
 */
int
YchannelRelease(Ychannel *channel);

/**
 * @brief Check if a Ychannel is readable
 * @ingroup yosal
 *
 * Check if a Ychannel is an input one, from which it's possible
 * to read or fetch data.
 *
 * @param channel Ychannel object
 * @retval 1 channel is readable
 * @retval 0 channel is not readable
 */
int
YchannelReadable(Ychannel *channel);

/**
 * @brief Check if a Ychannel is writable
 * @ingroup yosal
 *
 * Check if a Ychannel is an output one, into which it's possible to
 * to write data
 *
 * @param channel Ychannel object
 * @retval 1 channel is writable
 * @retval 0 channel is not writable
 */
int
YchannelWritable(Ychannel *channel);

/**
 * Determine the length of a Ychannel. This method might or might not return
 * a length depending on the underlying data.
 *
 * @param channel
 *
 * @return length of the Ychannel or YCHANNEL_NO_LENGTH
 */
uint64_t YchannelGetLength(Ychannel* channel);

/**
 * Set the length of a Ychannel. This can be used after creating a Ychannel with
 * a known amount of data available for reading.
 *
 * @param channel
 * @param length
 *
 * @return YOSAL_OK on success
 */
int YchannelSetLength(Ychannel* channel, uint64_t length);

/**
 * Make a Ychannel give up all references to its current memory buffer. This method
 * is useful if a Ychannel should be released but its buffer is still in use,
 * for example after wrapping a char array with a Ychannel temporarily.
 *
 * @param channel
 *
 * @return YOSAL_OK on success
 */
int YchannelResetBuffer(Ychannel* channel);

/**
 * Reset the length of a Ychannel to YCHANNEL_NO_LENGTH.
 *
 * @param channel
 *
 * @return YOSAL_OK on success
 */
int YchannelResetLength(Ychannel* channel);


/**
 * Check if a Ychannel has any data left to be read.
 *
 * @param channel
 *
 * @return YTRUE or YFALSE
 */
YBOOL
YchannelEof(Ychannel *channel);

/**
 * Fetch nbytes from a Ychannel. Same as read, but the memory being returned is
 * still owned by the Ychannel.
 *
 * @param channel
 * @param nbytes number of bytes to be read
 * @param[out] olengthptr number of bytes that were actually read
 *
 * @return reference to data that has been read
 */
const char*
YchannelFetch(Ychannel *channel, int nbytes, int *olengthptr);

/**
 * Skip n bytes when reading from a Ychannel
 *
 * @param channel
 * @param n number of bytes to skip
 *
 * @return number of bytes that were actually skipped
 */
int
YchannelSkip(Ychannel *channel, int n);

/**
 * Read nbytes from a Ychannel. Same as fetch, but the memory being returend is
 * owned and has to be freed by the caller. The caller has to ensure that the
 * size of buf is greater or equal to nbytes.
 *
 * @param channel
 * @param buf allocated buffer to read into
 * @param number of bytes to be read
 *
 * @return number of bytes that were actually read
 */
int
YchannelRead(Ychannel *channel, void *buf, int nbytes);

/**
 * Write at most towrite bytes from buffer buf into a Ychannel.
 *
 * @param channel
 * @param buf
 * @param towrite
 *
 * @return number of bytes actually written
 */
int
YchannelWrite(Ychannel *channel, void *buf, int towrite);

/**
 * Flush a Ychannel, @see YchannelFlushCB
 *
 * @param channel
 *
 * @return YOSAL_OK on success
 */
int
YchannelFlush(Ychannel *channel);

/**
 * Obtain a reference to the engine data of a channel. Typically this function is
 * only being used by engine implementations.
 *
 * @param channel
 *
 * @return reference to the data of the Ychannel
 */
void*
YchannelGetEngine(Ychannel *channel);

/**
 * Create a new Ychannel using an engine provided by the caller.
 *
 * @param name of the Ychannel
 * @param enginedata data that the Ychannel provides an interfce to
 * @param readcb engine read function
 * @param writecb engine write function
 * @param flushcb engine flushcb function
 * @param releasecb engine release function
 *
 * @return new Ychannel
 */
Ychannel*
YchannelInitGeneric(const char *name, void *enginedata,
                    YchannelReadCB readcb, YchannelWriteCB writecb,
                    YchannelFlushCB flushcb, YchannelReleaseCB releasecb);

#ifdef __cplusplus
};
#endif

#endif /* _YOSAL_YCHANNEL_H */
