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

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#include <pthread.h>

static int gYosal_ready = -1;
static pthread_mutex_t gYosal_mutex = PTHREAD_MUTEX_INITIALIZER;

int
Yosal_init()
{
 if (gYosal_ready < 0) {
    pthread_mutex_lock(&gYosal_mutex);
    if (gYosal_ready < 0) {
      Yrandom_init();
      Ytime_init();

      gYosal_ready = 1;
    }
    pthread_mutex_unlock(&gYosal_mutex);
 }

 return gYosal_ready;
}
