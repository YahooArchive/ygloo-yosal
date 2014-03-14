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
#include <stdio.h>
#include <errno.h>

typedef struct {
  FILE *file;
} YchannelFile;

static int
YchannelFileRead(Ychannel *channel, void *readbuf, int nbytes)
{
  YchannelFile *engine;
  size_t n;

  engine = (YchannelFile*) YchannelGetEngine(channel);
  if (engine == NULL) {
    return -1;
  }
  if (engine->file == NULL) {
    return -1;
  }

  n = fread(readbuf, 1, nbytes, engine->file);
  if (n <= 0) {
    if (feof(engine->file)) {
      /* Returning 0 bytes will signal EOF */
      return -1;
    }
  }

  return n;
}

static int
YchannelFileWrite(Ychannel *channel, void *buf, int nbytes)
{
  YchannelFile *engine;
  size_t n;

  engine = (YchannelFile*) YchannelGetEngine(channel);
  if (engine == NULL) {
    return -1;
  }
  if (engine->file == NULL) {
    return -1;
  }

  n = fwrite(buf, 1, nbytes, engine->file);
  if (n == 0) {
    return -1;
  }

  return n;
}

static int
YchannelFileFlush(Ychannel *channel)
{
  YchannelFile *engine;

  engine = (YchannelFile*) YchannelGetEngine(channel);
  if (engine == NULL) {
    return -1;
  }

  if (engine->file != NULL) {
    if (fflush(engine->file) != 0) {
      return -1;
    }
  }

  return 0;
}

static int
YchannelFileRelease(Ychannel *channel)
{
  YchannelFile *engine;

  engine = (YchannelFile*) YchannelGetEngine(channel);
  if (engine == NULL) {
    return -1;
  }

  if (engine->file != NULL) {
    if (YchannelWritable(channel)) {
      YchannelFileFlush(channel);
    }
    if (YchannelGetAutoRelease(channel)) {
      fclose(engine->file);
    }
    engine->file = NULL;
  }

  Ymem_free(engine);

  return 0;
}

Ychannel*
YchannelInitFile(FILE *file, int writable)
{
  YchannelFile *engine;
  Ychannel *channel;

  engine = (YchannelFile*) Ymem_malloc(sizeof(YchannelFile));
  if (engine == NULL) {
    return NULL;
  }

  engine->file = file;

  if (writable) {
    channel = YchannelInitGeneric("file", engine,
                                  NULL, YchannelFileWrite,
                                  YchannelFileFlush, YchannelFileRelease);
  } else {
    channel = YchannelInitGeneric("file", engine,
                                  YchannelFileRead, NULL,
                                  NULL, YchannelFileRelease);
  }

  if (channel == NULL) {
    Ymem_free(engine);
  }

  return channel;
}
