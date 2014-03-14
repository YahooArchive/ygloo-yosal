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

#include <time.h>
#include <sys/time.h>

#undef HAVE_POSIX_CLOCKS
#undef HAVE_MACH_TIME

#ifdef __BIONIC__
#define HAVE_POSIX_CLOCKS 1
#endif
#ifdef __APPLE__
#define HAVE_MACH_TIME 1
#endif

#if defined(HAVE_MACH_TIME)
#include <mach/mach_time.h>

static mach_timebase_info_data_t mach_time_info = {
  .numer = 0,
  .denom = 0
};
#endif

int
Ytime_init()
{
#if defined(HAVE_MACH_TIME)
  if (mach_timebase_info(&mach_time_info) != KERN_SUCCESS) {
    mach_time_info.numer = 0;
    mach_time_info.denom = 0;
  }
#endif

  return YOSAL_OK;
}

nsecs_t
Ytime(int clock)
{
  struct timeval t;

#if defined(HAVE_POSIX_CLOCKS)
  static const clockid_t clocks[] = {
    CLOCK_REALTIME,
    CLOCK_MONOTONIC,
    CLOCK_PROCESS_CPUTIME_ID,
    CLOCK_THREAD_CPUTIME_ID
  };
  struct timespec mt;
  clockid_t tclock;

  if (clock == YTIME_CLOCK_MONOTONIC) {
    tclock = clocks[1];
  } else {
    tclock = clocks[0];
  }

  mt.tv_sec = 0;
  mt.tv_nsec = 0;
  if (clock_gettime(tclock, &mt) == 0) {
    return ( ((nsecs_t) mt.tv_sec)*1000000000LL + ((nsecs_t) mt.tv_nsec) );
  }
#endif
#if defined(HAVE_MACH_TIME)
  if (clock == YTIME_CLOCK_MONOTONIC) {
    Yosal_init();
    if (mach_time_info.denom != 0) {
      uint64_t elapsed = mach_absolute_time ();
      uint64_t nanos = (elapsed * mach_time_info.numer) / mach_time_info.denom;
      return (nsecs_t) nanos;
    }
  }
#endif

  /* Fallback to non-monotonic gettimeofday() */
  t.tv_sec = t.tv_usec = 0;
  gettimeofday(&t, NULL);

  return ( ((nsecs_t) t.tv_sec)*1000000000LL + ((nsecs_t) t.tv_usec)*1000LL );
}

ytime_t Ytime_epoch() {
  return (ytime_t) (Ytime(YTIME_CLOCK_REALTIME)/YOSAL_NS_PER_SECOND);
}
