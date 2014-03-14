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
#include "yosal/ytest.h"

#include <stdio.h>

static int
Ytest_check(int check, int fatal, const char *msg)
{
  if (!check) {
    fprintf(stderr, "test failed: ");
    fprintf(stderr, "%s\n", msg);
    fflush(stderr);
    if (fatal) exit(1);
  }
  return 0;
}

int
Ytest_check_true(int fatal, int value, const char *msg)
{
  return Ytest_check(value, fatal, msg);
}

int
Ytest_check_false(int fatal, int value, const char *msg)
{
  return Ytest_check(!value, fatal, msg);
}

int
Ytest_check_eq(int fatal, int value1, int value2, const char *msg)
{
  return Ytest_check(value1 == value2, fatal, msg);
}

int
Ytest_check_ne(int fatal, int value1, int value2, const char *msg)
{
  return Ytest_check(value1 != value2, fatal, msg);
}

int
Ytest_check_streq(int fatal, const char *value1, const char *value2,
                  const char *msg)
{
  return Ytest_check(strcmp(value1, value2) == 0, fatal, msg);
}

int
Ytest_check_strne(int fatal, const char *value1, const char *value2,
                  const char *msg)
{
  return Ytest_check(strcmp(value1, value2) != 0, fatal, msg);
}

int
Ytest_check_isnull(int fatal, void *value1, const char *msg)
{
  return Ytest_check(value1 == NULL, fatal, msg);
}

int
Ytest_check_memeq(int fatal, const char *value1, const char *value2,
                  int len, const char *msg)
{
  return Ytest_check(memcmp(value1, value2, len) == 0, fatal, msg);
}

int
Ytest_check_memne(int fatal, const char *value1, const char *value2,
                  int len, const char *msg)
{
  return Ytest_check(memcmp(value1, value2, len) != 0, fatal, msg);
}
