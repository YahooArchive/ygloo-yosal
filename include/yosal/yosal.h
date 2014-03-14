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

#ifndef _YOSAL_H
#define _YOSAL_H 1

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define YOSAL_OK    ((int)  0)
#define YOSAL_ERROR ((int) -1)

#ifndef container_of
#define container_of(ptr, type, member) (type *)((char *)(ptr) - offsetof(type, member))
#endif

#define YPRIVATE(x) _yosal_##x

#include "yosal/android.h"
#include "yosal/endian.h"
#include "yosal/system.h"

#include "yosal/yoptim.h"

#include "yosal/yalloc.h"

#include "yosal/hash.h"
#include "yosal/digest.h"
#include "yosal/base64.h"

#include "yosal/ybool.h"
#include "yosal/yobject.h"
#include "yosal/hashmap.h"
#include "yosal/queue.h"
#include "yosal/array.h"

#include "yosal/yalloc.h"
#include "yosal/ybuffer.h"
#include "yosal/ychannel.h"

#include "yosal/ytime.h"
#include "yosal/yrandom.h"

#include "yosal/ytest.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
};
#endif

#endif /* _YOSAL_H */
