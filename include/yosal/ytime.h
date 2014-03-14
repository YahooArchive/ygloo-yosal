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

#ifndef _YOSAL_YTIME_H
#define	_YOSAL_YTIME_H

#ifdef	__cplusplus
extern "C" {
#endif

#define YOSAL_MS_PER_SECOND ((uint64_t) 1000LL)
#define YOSAL_US_PER_SECOND ((uint64_t) 1000000LL)
#define YOSAL_NS_PER_SECOND ((uint64_t) 1000000000LL)

/**
 * @defgroup Ytime
 *
 * @brief Support for epoch time and nanosecond accuracy clocks
 *
 * @{
 */

/**
 * nanoseconds since Epoch
 */
#define YTIME_CLOCK_REALTIME  0

/**
 * nanoseconds since an arbitrary time in the past, monotic.
 */
#define YTIME_CLOCK_MONOTONIC 1

typedef uint64_t ytime_t;
typedef int64_t nsecs_t;


/**
 * Initialize time interface
 * Call this method at least once before calling other Ytime function, either directly
 * from the "main" thread, or through a call to Yosal_Init
 *
 * @return YOSAL_OK on success
 */
int Ytime_init();

/**
 * Obtain Unix timestamp
 * @return seconds since 0 hours, 0 minutes, 0 seconds, January 1, 1970
 */
ytime_t Ytime_epoch();

/**
 * Obtain the time of a given clock.
 * @param clockmode YTIME_CLOCK_REALTIME or YTIME_CLOCK_MONOTONIC
 */
nsecs_t
Ytime(int clockmode);

#ifdef	__cplusplus
}
#endif

#endif	/* _YOSAL_YTIME_H */

