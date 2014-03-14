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

#define LOG_TAG "yosal::random"

#include "yosal/yosal.h"
#include <unistd.h>
#include <fcntl.h>

#define YOSAL_URANDOM_DEVICE "/dev/urandom"

int
Yrandom_init()
{
  long seed = 0;

  /* attempt to use random device */
  if (access(YOSAL_URANDOM_DEVICE, R_OK) == 0) {
    int fd;

    fd = open(YOSAL_URANDOM_DEVICE, O_RDONLY);
    if (fd >= 0) {
      long l;
      if (read(fd, &l, sizeof(long)) == sizeof(long)) {
        seed = l;
      };
      close(fd);
    }
  }

  if (seed == 0) {
    ALOGD("reading from urandom device failed");
  }

  /* fallback for systems without random device */
  seed = seed ^ ((long) Ytime(YTIME_CLOCK_REALTIME) * 11 + getpid());

  srand48(seed);

  return YOSAL_OK;
}

uint32_t
Yrandom()
{
  Yosal_init();

  return (uint32_t) lrand48();
}

int
Yrandom_hexstring(char* outbuf, int len)
{
  int printed;

  if (outbuf == NULL) {
    return YOSAL_ERROR;
  }

  for (printed=0; printed < len;) {
    uint32_t r = Yrandom();

    snprintf(outbuf+printed, len-printed, "%08lx", (unsigned long) r);
    printed += 8;
  }

  return YOSAL_OK;
}
