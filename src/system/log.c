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
#include <stdarg.h>
#include <stdio.h>

#include "yosal/yosal.h"

static int loglevel = ANDROID_LOG_DEBUG;

void __android_set_loglevel(int prio)
{
    loglevel = prio;
}

#if YOSAL_CONFIG_ANDROID_EMULATION

int __android_log_print(int prio, const char *tag,  const char *fmt, ...)
{
  va_list ap;
  int rc = 0;

  if (prio < loglevel) {
      return rc;
  }

  fprintf(stderr, "[%llu] %s:\t", (unsigned long long)Ytime_epoch(), tag);

  va_start(ap, fmt);
  rc = vfprintf(stderr, fmt, ap);
  va_end(ap);

  fwrite("\n", 1, 1, stderr);

  return rc;
}

#endif


