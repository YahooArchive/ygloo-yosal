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

#include <unistd.h>
#include <pthread.h>
#include <errno.h>

/* Size of read buffer for prefetching */
#define INPUT_BUF_SIZE   (16*1024)
#define OUTPUT_BUF_SIZE  (16*1024)

#define YCHANNEL_READ  0
#define YCHANNEL_WRITE 1

#define YCHANNEL_NO_LENGTH ((uint64_t) -1)


/* Expanded data source object for stdio and stream input */
struct YchannelStruct {
  /** @privatesection */
  int rwmode;

  /* length (optional) */
  uint64_t inlength;
  uint64_t incount;

  /* Push-back buffer */
  const char *pbuf;
  uint32_t ppos;
  uint32_t plength;

  /* Static header */
  const char *hbuf;
  uint32_t hpos;
  uint32_t hlength;

  /* Read (prefetch) buffer */
  char *rbuf;
  uint32_t rpos;
  uint32_t rlength;
  uint32_t rsize;

  YBOOL terminated;
  int autorelease;

  /* Backend private data */
  void *enginedata;

  YchannelReadCB readcb;
  YchannelWriteCB writecb;
  YchannelFlushCB flushcb;
  YchannelReleaseCB releasecb;
};

static Ychannel*
YchannelInit()
{
    Ychannel *channel = NULL;

    channel = (Ychannel*) Ymem_malloc(sizeof(Ychannel));
    if (channel == NULL) {
        return NULL;
    }

    memset(channel, 0, sizeof(Ychannel));

    channel->rwmode = YCHANNEL_READ;

    channel->inlength = YCHANNEL_NO_LENGTH;
    channel->incount = 0;

    channel->hbuf = NULL;
    channel->hlength = 0;
    channel->hpos = 0;

    channel->pbuf = NULL;
    channel->ppos = 0;
    channel->plength = 0;

    channel->rbuf = NULL;
    channel->rlength = 0;
    channel->rpos = 0;
    channel->rsize = 0;

    channel->terminated = YFALSE;
    channel->autorelease = 0;

    channel->enginedata = NULL;

    return channel;
}

int
YchannelSetAutoRelease(Ychannel* channel, int autorelease)
{
  if (channel == NULL) {
    return YOSAL_ERROR;
  }

  channel->autorelease = autorelease;
  return YOSAL_OK;
}

int
YchannelGetAutoRelease(Ychannel* channel) {
  if (channel == NULL) {
    return 0;
  }

  return channel->autorelease;
}

int
YchannelResetLength(Ychannel* channel) {
  channel->inlength = YCHANNEL_NO_LENGTH;
  return YOSAL_OK;
}

int
YchannelSetLength(Ychannel* channel, uint64_t length) {

  channel->inlength = length;

  return YOSAL_OK;
}

int
YchannelResetBuffer(Ychannel* channel)
{
  if (channel == NULL) {
    return YOSAL_OK;
  }

  channel->hbuf = NULL;
  channel->hlength = 0;
  channel->hpos = 0;
  return YOSAL_OK;
}

uint64_t
YchannelGetLength(Ychannel* channel) {
  int length = channel->inlength;

  return length;
}

Ychannel*
YchannelInitByteArray(const char *header, int hlength)
{
    Ychannel *channel;

    channel = YchannelInit();
    if (channel != NULL) {
        channel->rwmode = YCHANNEL_READ;
        channel->hbuf = header;
        channel->hlength = hlength;
        channel->hpos = 0;
    }

    return channel;
}

int
YchannelRelease(Ychannel *channel)
{
  if (channel != NULL) {
    if (channel->rbuf != NULL) {
      Ymem_free(channel->rbuf);
      channel->rbuf = NULL;
    }
    if (channel->hbuf != NULL) {
      Ymem_free((void*)channel->hbuf);
      channel->hbuf = NULL;
    }
    if (channel->releasecb != NULL) {
      channel->releasecb(channel);
    }

    Ymem_free(channel);
  }

  return 0;
}

int
YchannelReadable(Ychannel *channel)
{
  if (channel == NULL || channel->rwmode != YCHANNEL_READ) {
    return 0;
  }

  return 1;
}

int
YchannelWritable(Ychannel *channel)
{
  if (channel == NULL || channel->rwmode != YCHANNEL_WRITE) {
    return 0;
  }

  return 1;
}

YBOOL
YchannelEof(Ychannel *channel)
{
  if (!YchannelReadable(channel)) {
    return YTRUE;
  }

  if (channel->plength > 0 && channel->ppos < channel->plength) {
    /* Push-back buffer not empty */
    return YFALSE;
  }
  if (channel->hlength > 0 && channel->hpos < channel->hlength) {
    /* Header not empty */
    return YFALSE;
  }
#if 0
  if (channel->clength > 0 && channel->cpos < channel->clength) {
    /* Compressed buffer not empty */
    return YFALSE;
  }
  if (channel->dlength > 0 && channel->dpos < channel->dlength) {
    /* Uncompressed input buffer not empty */
    return YFALSE;
  }
#else
  if (channel->rlength > 0 && channel->rpos < channel->rlength) {
    /* Read buffer not empty */
    return YFALSE;
  }
#endif

  return channel->terminated;
}

static int
YchannelReadDirect(Ychannel *channel, void *buf, int nbytes)
{
  if (channel == NULL || channel->readcb == NULL || nbytes <= 0) {
    return 0;
  }

  return channel->readcb(channel, buf, nbytes);
}

static const char*
YchannelFetchData(Ychannel *channel, int nbytes, int *olengthptr, int doread)
{
  const char *result = NULL;
  int olength = 0;

  if (!YchannelReadable(channel)) {
    return NULL;
  }

  if (nbytes < 0) {
    nbytes = 0;
  }

  if (channel->inlength != YCHANNEL_NO_LENGTH) {
    if (channel->inlength < nbytes) {
      nbytes = (int) channel->inlength;
    }
    if (channel->inlength - nbytes < channel->incount) {
      nbytes = channel->inlength - channel->incount;
    }
  }

  if (nbytes > 0) {
    if (channel->plength > 0 && channel->ppos < channel->plength) {
      /* Return content from push-back buffer */
      result = channel->pbuf + channel->ppos;
      olength = channel->plength - channel->ppos;
      if (olength > nbytes) {
        olength = nbytes;
      }
      channel->ppos += olength;
    } else if (channel->hlength > 0 && channel->hpos < channel->hlength) {
      /* Return content from static buffer */
      result = channel->hbuf + channel->hpos;
      olength = channel->hlength - channel->hpos;
      if (olength > nbytes) {
        olength = nbytes;
      }
      channel->hpos += olength;
    } else {
      if (channel->rlength <= 0 || channel->rpos >= channel->rlength) {
        /* Empty current read buffer */
        channel->rlength = 0;
        channel->rpos = 0;
        /* fetch more data from underlying input, and save it in read buffer */
        if (channel->rbuf == NULL) {
          channel->rbuf = Ymem_malloc(INPUT_BUF_SIZE);
          if (channel->rbuf != NULL) {
            channel->rsize = INPUT_BUF_SIZE;
          }
        }

        if (doread && channel->readcb != NULL) {
          int nread = YchannelReadDirect(channel, channel->rbuf, channel->rsize);
          if (nread < 0) {
            channel->terminated = YTRUE;
          } else {
            channel->rlength = nread;
          }
        }
      }

      if (channel->rlength > 0 && channel->rpos < channel->rlength) {
        /* Return content from read buffer */
        result = channel->rbuf + channel->rpos;
        olength = channel->rlength - channel->rpos;
        if (olength > nbytes) {
          olength = nbytes;
        }
        channel->rpos += olength;
      }
    }
  }

  if (olengthptr != NULL) {
    *olengthptr = olength;
  }

  channel->incount += olength;

  return result;
}

const char*
YchannelFetch(Ychannel *channel, int nbytes, int *olengthptr)
{
  return YchannelFetchData(channel, nbytes, olengthptr, YTRUE);
}

/* An alternative to fetch, copying the read data into a buffer given by the caller */
int
YchannelRead(Ychannel *channel, void *buf, int toread)
{
  const char *chunk;
  int chunklen;
  int nbytes;
  char *nextc = (char*) buf;
  int directio;

  if (!YchannelReadable(channel)) {
    return 1;
  }

  if (toread <= 0) {
    return 0;
  }

  nbytes = 0;

  /* Get as many bytes as possible from prefetched buffer */
  while (toread > 0) {
    chunk = YchannelFetchData(channel, toread, &chunklen, YFALSE);
    if (chunk == NULL || chunklen <= 0) {
      break;
    }

    if (nextc != NULL) {
      memcpy(nextc, chunk, chunklen);
      nextc += chunklen;
    }
    toread -= chunklen;
    nbytes += chunklen;
  }

  if (toread <= 0) {
    return nbytes;
  }

  /* If it is safe to do direct I/O if all internal buffers are empty */
  directio = YFALSE;
  if ( (nextc != NULL) &&
       (channel->plength <= 0 || channel->ppos >= channel->plength) &&
       (channel->hlength <= 0 || channel->hpos >= channel->hlength) &&
       (channel->rlength <= 0 || channel->rpos >= channel->rlength) ) {
    directio = YTRUE;
  }

  if (directio) {
    /* If this is not a transformation channel, read data directly from I/O engine */
    while (toread > 0) {
      chunklen = YchannelReadDirect(channel, nextc, toread);
      if (chunklen <= 0) {
        break;
      }
      toread -= chunklen;
      nbytes += chunklen;
    }
  }

  /* Get missing chunk using standard fetch method */
  while (toread > 0) {
    chunk = YchannelFetch(channel, toread, &chunklen);
    if (chunk == NULL || chunklen <= 0) {
      break;
    }

    if (nextc != NULL) {
      memcpy(nextc, chunk, chunklen);
      nextc += chunklen;
    }
    toread -= chunklen;
    nbytes += chunklen;
  }

  return nbytes;
}

/* Skip bytes from channel. This is not optimized for large skip,
   but doesn't require underlying input channel to be seekable */
int
YchannelSkip(Ychannel *channel, int nbytes)
{
  return YchannelRead(channel, NULL, nbytes);
}

int
YchannelWrite(Ychannel *channel, void *buf, int towrite)
{
  jint written;
  char *nextc;
  int nbytes;

  if (!YchannelWritable(channel)) {
    return -1;
  }

  if (buf == NULL || towrite <= 0) {
    return 0;
  }

  written = 0;
  nextc = (char*) buf;

  if (channel->writecb != NULL) {
    while (written < towrite) {
      nbytes = channel->writecb(channel, nextc, towrite - written);
      if (nbytes < 0) {
        /* Write failed, no retry */
        break;
      }
      written += nbytes;
      nextc += nbytes;
    }
  }

  return written;
}

int
YchannelFlush(Ychannel *channel)
{
  if (!YchannelWritable(channel)) {
    return YOSAL_ERROR;
  }

  if (channel->flushcb != NULL) {
    return channel->flushcb(channel);
  }

  return YOSAL_OK;
}

void*
YchannelGetEngine(Ychannel *channel)
{
  if (channel == NULL) {
    return NULL;
  }

  return channel->enginedata;
}

Ychannel*
YchannelInitGeneric(const char *name, void *enginedata,
                    YchannelReadCB readcb, YchannelWriteCB writecb,
                    YchannelFlushCB flushcb, YchannelReleaseCB releasecb)
{
  Ychannel *channel;

  channel = YchannelInit();
  if (channel == NULL) {
    return NULL;
  }

  if (writecb != NULL) {
    channel->rwmode = YCHANNEL_WRITE;
  } else {
    channel->rwmode = YCHANNEL_READ;
  }

  channel->enginedata = enginedata;
  channel->readcb = readcb;
  channel->writecb = writecb;
  channel->flushcb = flushcb;
  channel->releasecb = releasecb;

  return channel;
}
