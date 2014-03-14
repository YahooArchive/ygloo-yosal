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

/**
 * Very minimalist test framework for native SDK
 */
#ifndef _YOSAL_YTEST_H
#define _YOSAL_YTEST_H 1

#define YTEST_EXPECT_VALUE 0
#define YTEST_ASSERT_VALUE 1

#include "yosal/yosal.h"
#include "yosal/ytest.h"

#include <stdio.h>

int Ytest_check_true(int fatal, int value, const char *msg);

int Ytest_check_false(int fatal, int value, const char *msg);

int Ytest_check_eq(int fatal, int value1, int value2, const char *msg);

int Ytest_check_ne(int fatal, int value1, int value2, const char *msg);

int Ytest_check_streq(int fatal, const char *value1, const char *value2, const char *msg);

int Ytest_check_strne(int fatal, const char *value1, const char *value2, const char *msg);

int Ytest_check_isnull(int fatal, void *value1, const char *msg);

int Ytest_check_memeq(int fatal, const char *value1, const char *value2, int len, const char *msg);

int Ytest_check_memne(int fatal, const char *value1, const char *value2, int len, const char *msg);


#define YTEST_ASSERT_TRUE(x) Ytest_check_true(1, x, #x)
#define YTEST_EXPECT_TRUE(x) Ytest_check_true(0, x, #x)

#define YTEST_ASSERT_FALSE(x) Ytest_check_false(1, x, #x)
#define YTEST_EXPECT_FALSE(x) Ytest_check_false(0, x, #x)

#define YTEST_ASSERT_EQ(x, y) Ytest_check_eq(1, x != y, 0, #x " == " #y)
#define YTEST_EXPECT_EQ(x, y) Ytest_check_eq(0, x != y, 0, #x " == " #y)

#define YTEST_ASSERT_NE(x, y) Ytest_check_ne(1, x != y, 0, #x " != " #y)
#define YTEST_EXPECT_NE(x, y) Ytest_check_ne(0, x != y, 0, #x " != " #y)

#define YTEST_ASSERT_STREQ(x, y) Ytest_check_streq(1, x, y, "streq(" #x "," #y ")")
#define YTEST_EXPECT_STREQ(x, y) Ytest_check_streq(0, x, y, "streq(" #x "," #y ")")

#define YTEST_ASSERT_STRNE(x, y) Ytest_check_strne(1, x, y, "strne(" #x "," #y ")")
#define YTEST_EXPECT_STRNE(x, y) Ytest_check_strne(0, x, y, "strne(" #x "," #y ")")

#define YTEST_ASSERT_ISNULL(x) Ytest_check_isnull(1, x, "is null " #x)
#define YTEST_EXPECT_ISNULL(x) Ytest_check_isnull(0, x, "is null " #x)

#define YTEST_ASSERT_MEMEQ(x, y, n) Ytest_check_memeq(1, x, y, n, "memeq(" #x "," #y ")")
#define YTEST_EXPECT_MEMEQ(x, y, n) Ytest_check_memeq(0, x, y, n, "memeq(" #x "," #y ")")

#define YTEST_ASSERT_MEMNE(x, y, n) Ytest_check_memne(1, x, y, n, "memne(" #x "," #y ")")
#define YTEST_EXPECT_MEMNE(x, y, n) Ytest_check_memne(0, x, y, n, "memne(" #x "," #y ")")

#endif /* _YOSAL_YTEST_H */
