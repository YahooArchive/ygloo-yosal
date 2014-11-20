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
#include <fcntl.h>
#include <errno.h>

typedef struct {
  int fd;
} YchannelFd;

static int
YchannelFdRead(Ychannel *channel, void *readbuf, int nbytes)
{
  YchannelFd *engine;
  int nread = 0;

  engine = (YchannelFd*) YchannelGetEngine(channel);
  if (engine == NULL) {
    return -1;
  }
  if (engine->fd < 0) {
    return -1;
  }
  if (nbytes <= 0) {
    return 0;
  }

  while (1) {
    ssize_t n = read(engine->fd, readbuf, nbytes);
    if (n == 0) {
      /* EOF */
    }
    else if (n <  0) {
      if (errno == EAGAIN || errno == EINTR) {
        /* Retry */
        continue;
      }
    } else {
      nread += n;
    }
    break;
  }

  return nread;
}

static int
YchannelFdWrite(Ychannel *channel, const void *buf, int nbytes)
{
  YchannelFd *engine;
  ssize_t n;

  engine = (YchannelFd*) YchannelGetEngine(channel);
  if (engine == NULL) {
    return -1;
  }
  if (engine->fd < 0) {
    return -1;
  }
  if (nbytes <= 0) {
    return 0;
  }

  n = write(engine->fd, buf, nbytes);
  if (n <  0) {
    if (errno == EAGAIN || errno == EINTR) {
      /* Retry */
      return 0;
    }
    return -1;
  }

  return n;
}

static int
YchannelFdRelease(Ychannel *channel)
{
  YchannelFd *engine;

  engine = (YchannelFd*) YchannelGetEngine(channel);
  if (engine == NULL) {
    return -1;
  }

  if (engine->fd >= 0) {
    if (YchannelGetAutoRelease(channel)) {
      close(engine->fd);
    }
    engine->fd = -1;
  }

  Ymem_free(engine);

  return 0;
}

Ychannel*
YchannelInitFd(int fd, int writable)
{
  YchannelFd *engine;
  Ychannel *channel;

  engine = (YchannelFd*) Ymem_malloc(sizeof(YchannelFd));
  if (engine == NULL) {
    return NULL;
  }

  engine->fd = fd;

  if (writable) {
    channel = YchannelInitGeneric("fd", engine,
                                  NULL, YchannelFdWrite,
                                  NULL, YchannelFdRelease);
  } else {
    channel = YchannelInitGeneric("fd", engine,
                                  YchannelFdRead, NULL,
                                  NULL, YchannelFdRelease);
  }
  if (channel == NULL) {
    Ymem_free(engine);
  }

  return channel;
}
