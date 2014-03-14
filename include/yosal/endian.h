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

#ifndef _YOSAL_ENDIAN_H
#define _YOSAL_ENDIAN_H 1

#include <yosal/yosal.h>

#include <sys/param.h>

#ifdef __BIONIC__
#include <endian.h>
#endif

#ifndef YOSAL_BIG_ENDIAN
#if defined(__BIG_ENDIAN)
#  define  YOSAL_BIG_ENDIAN __BIG_ENDIAN
#elif defined(BIG_ENDIAN)
#  define  YOSAL_BIG_ENDIAN BIG_ENDIAN
#elif defined(_BIG_ENDIAN)
#  define  YOSAL_BIG_ENDIAN _BIG_ENDIAN
#elif defined(__BIG_ENDIAN__)
#  define YOSAL_BIG_ENDIAN __BIG_ENDIAN__
#else
#  define YOSAL_BIG_ENDIAN 4321
#endif
#endif

#ifndef YOSAL_LITTLE_ENDIAN
#if defined(__LITTLE_ENDIAN)
#  define  YOSAL_LITTLE_ENDIAN __LITTLE_ENDIAN
#elif defined(LITTLE_ENDIAN)
#  define  YOSAL_LITTLE_ENDIAN LITTLE_ENDIAN
#elif defined(_LITTLE_ENDIAN)
#  define  YOSAL_LITTLE_ENDIAN _LITTLE_ENDIAN
#elif defined(__LITTLE_ENDIAN__)
#  define YOSAL_LITTLE_ENDIAN __LITTLE_ENDIAN__
#else
#  define YOSAL_LITTLE_ENDIAN 1234
#endif
#endif

#ifndef YOSAL_BYTE_ORDER
#if defined(__BYTE_ORDER)
#  define  YOSAL_BYTE_ORDER __BYTE_ORDER
#elif defined(BYTE_ORDER)
#  define  YOSAL_BYTE_ORDER BYTE_ORDER
#elif defined(_BYTE_ORDER)
#  define  YOSAL_BYTE_ORDER _BYTE_ORDER
#elif defined(__BYTE_ORDER__)
#  define YOSAL_BYTE_ORDER __BYTE_ORDER__
#elif defined(__i386__) || defined(i386) || defined(__x86_64__) || \
      defined(__i486__) || defined(__i586__) || defined(__i686__)
#  define YOSAL_BYTE_ORDER YOSAL_LITTLE_ENDIAN
#elif defined(__arm__) || defined(__arm) || defined(arm)
#  define YOSAL_BYTE_ORDER YOSAL_LITTLE_ENDIAN
#elif defined(sparc) || defined(POWERPC) || defined(mc68000)
#  define YOSAL_BYTE_ORDER YOSAL_BIG_ENDIAN
#else
#  error "Unable to determine endianess for target platform"
#endif
#endif

#if YOSAL_BYTE_ORDER == YOSAL_LITTLE_ENDIAN
#define YOSAL_ARCH_LITTLE_ENDIAN 1
#define YOSAL_ARCH_BIG_ENDIAN 0
#else
#define YOSAL_ARCH_LITTLE_ENDIAN 0
#define YOSAL_ARCH_BIG_ENDIAN 1
#endif

#endif /* _YOSAL_ENDIAN_H */
