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

#ifndef _YOSAL_SYSTEM_H
#define _YOSAL_SYSTEM_H 1

#include <yosal/yosal.h>

#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize all static global state for Yosal API.
 * Call this method may be called one at start up by the customer of
 * the library. Any call to a function which requires initialization
 * will otherwise automatically call it, if library is not initialized
 * yet, so this can safely be ignored, or called many times.
 *
 * @return YOSAL_OK on success
 */
int
Yosal_init();

#ifdef __cplusplus
};
#endif

#endif /* _YOSAL_SYSTEM_H */
