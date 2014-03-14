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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int
usage()
{
  fprintf(stderr, "usage: test-yosal\n");

  return 0;
}

static int
test_hashmap()
{
  Yhashmap *map;
  YhashmapEntry *entry;
  int isnew, len;

  printf("Test yosal::hashmap\n");

  map = Yhashmap_create(32);
  YTEST_EXPECT_TRUE(map != NULL);
  YTEST_EXPECT_EQ(Yhashmap_size(map), 0);

  entry = Yhashmap_put(map, "key1", -1, &isnew);

  YTEST_EXPECT_TRUE(isnew != 0);
  YTEST_EXPECT_FALSE(isnew == 0);
  Yhashmap_setvalue(entry, "value1", -1);

  YTEST_EXPECT_MEMEQ(Yhashmap_key(Yhashmap_get(map, "key1", -1), &len), "key1", 4);
  YTEST_EXPECT_EQ(len, 4);
  YTEST_EXPECT_MEMEQ(Yhashmap_value(Yhashmap_get(map, "key1", -1), &len), "value1", 6);
  YTEST_EXPECT_EQ(len, 6);
  YTEST_EXPECT_EQ(Yhashmap_size(map), 1);

  entry = Yhashmap_put(map, "key2", -1, &isnew);
  YTEST_EXPECT_TRUE(isnew != 0);
  Yhashmap_setvalue(entry, "value2", -1);

  YTEST_EXPECT_MEMEQ(Yhashmap_key(Yhashmap_get(map, "key2", -1), &len), "key2", 4);
  YTEST_EXPECT_EQ(len, 4);
  YTEST_EXPECT_MEMEQ(Yhashmap_value(Yhashmap_get(map, "key2", -1), &len), "value2", 6);
  YTEST_EXPECT_EQ(len, 6);
  YTEST_EXPECT_EQ(Yhashmap_size(map), 2);

  entry = Yhashmap_put(map, "key3", -1, &isnew);
  YTEST_EXPECT_TRUE(isnew != 0);
  Yhashmap_setvalue(entry, "value3", -1);

  YTEST_EXPECT_MEMEQ(Yhashmap_key(Yhashmap_get(map, "key3", -1), &len), "key3", 4);
  YTEST_EXPECT_EQ(len, 4);
  YTEST_EXPECT_MEMEQ(Yhashmap_value(Yhashmap_get(map, "key3", -1), &len), "value3", 6);
  YTEST_EXPECT_EQ(len, 6);
  YTEST_EXPECT_EQ(Yhashmap_size(map), 3);

  entry = Yhashmap_put(map, "key4", -1, &isnew);
  YTEST_EXPECT_TRUE(isnew != 0);
  Yhashmap_setvalue(entry, "value4", -1);

  YTEST_EXPECT_MEMEQ(Yhashmap_key(Yhashmap_get(map, "key4", -1), &len), "key4", 4);
  YTEST_EXPECT_EQ(len, 4);
  YTEST_EXPECT_MEMEQ(Yhashmap_value(Yhashmap_get(map, "key4", -1), &len), "value4", 6);
  YTEST_EXPECT_EQ(len, 6);
  YTEST_EXPECT_EQ(Yhashmap_size(map), 4);

  entry = Yhashmap_put(map, "key2", 4, &isnew);
  YTEST_EXPECT_TRUE(isnew == 0);
  Yhashmap_setvalue(entry, "new_value_for_key2", -1);

  YTEST_EXPECT_MEMEQ(Yhashmap_key(Yhashmap_get(map, "key2", -1), &len), "key2", 4);
  YTEST_EXPECT_EQ(len, 4);
  YTEST_EXPECT_MEMEQ(Yhashmap_value(Yhashmap_get(map, "key2", -1), &len), "new_value_for_key2", 18);
  YTEST_EXPECT_EQ(len, 18);

  YTEST_EXPECT_EQ(Yhashmap_size(map), 4);

  Yhashmap_remove(map, Yhashmap_get(map, "key2", -1));

  YTEST_EXPECT_EQ(Yhashmap_get(map, "key2", -1), NULL);
  YTEST_EXPECT_FALSE(Yhashmap_contain(map, "key2", -1));
  YTEST_EXPECT_TRUE(Yhashmap_contain(map, "key3", -1));
  YTEST_EXPECT_EQ(Yhashmap_size(map), 3);

  Yhashmap_removekey(map, "key1", -1);

  YTEST_EXPECT_EQ(Yhashmap_get(map, "key1", -1), NULL);
  YTEST_EXPECT_EQ(Yhashmap_get(map, "key2", -1), NULL);
  YTEST_EXPECT_FALSE(Yhashmap_contain(map, "key1", -1));
  YTEST_EXPECT_FALSE(Yhashmap_contain(map, "key2", -1));
  YTEST_EXPECT_TRUE(Yhashmap_contain(map, "key3", -1));
  YTEST_EXPECT_EQ(Yhashmap_size(map), 2);

  Yhashmap_release(map);

  printf("Test passed\n");

  return 0;
}

static int
test_digest()
{
  char hmac[YOSAL_DIGEST_SHA1_HEX];
  char refhmac[YOSAL_DIGEST_SHA1_HEX];
  Ydigest *context;
  int i, j, len;
  char *input;
  int mode;

  printf("Test yosal::digest\n");

  mode = YOSAL_DIGEST_MODE_SHA1;
  context = Ydigest_create(mode);
  YTEST_EXPECT_TRUE(context != NULL);

  /* Test Vectors (from FIPS PUB 180-1) */

  /* "abc"
     -> a9993e364706816aba3e25717850c26c9cd0d89d */
  Ydigest_update(context, "abc", -1);
  Ydigest_final(context);
  Ydigest_hex(context, hmac);
  YTEST_EXPECT_STREQ(hmac, "a9993e364706816aba3e25717850c26c9cd0d89d");

  /* "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
     -> 84983e441c3bd26ebaae4aa1f95129e5e54670f1 */
  Ydigest_reset(context, mode);
  Ydigest_update(context, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", -1);
  Ydigest_final(context);
  Ydigest_hex(context, hmac);
  YTEST_EXPECT_STREQ(hmac, "84983e441c3bd26ebaae4aa1f95129e5e54670f1");

  /* A million repetitions of "a"
     -> 34aa973cd4c4daa4f61eeb2bdbad27316534016f */
  Ydigest_reset(context, mode);
  for (i = 0; i < 100000; i++) {
    Ydigest_update(context, "aaaaaaaaaa", 10);
  }
  Ydigest_final(context);
  Ydigest_hex(context, hmac);
  YTEST_EXPECT_STREQ(hmac, "34aa973cd4c4daa4f61eeb2bdbad27316534016f");

  Ydigest_release(context);

  mode = YOSAL_DIGEST_MODE_MD5;
  input = "to understand recursion, you must understand recursion";
  len = strlen(input);

  context = Ydigest_create(mode);
  Ydigest_update(context, input, -1);
  Ydigest_final(context);
  Ydigest_hex(context, refhmac);
  Ydigest_release(context);

  for (j = 1; j < len; j++) {
    int i;
    int step = j;

    context = Ydigest_create(mode);
    for (i = 0; i < len; i += step) {
      if (i + step > len) {
        step = len - i;
      }
      Ydigest_update(context, input + i, step);
    }
    Ydigest_final(context);

    Ydigest_hex(context, hmac);
    Ydigest_release(context);

    YTEST_EXPECT_STREQ(refhmac, hmac);
  }

  printf("Test passed\n");

  return 0;
}


static int
test_hmac()
{
  char signature_md5[YOSAL_DIGEST_MD5_HEX];
  char signature_sha1[YOSAL_DIGEST_SHA1_HEX];

  char* message = "Episode IV, A NEW HOPE It is a period of civil war.";
  char* key = "deadbeef";

  int mlen = strlen(message);
  int klen = strlen(key);

  printf("Test yosal::Ydigest hmac-md5\n");
  Ydigest* dig = Ydigest_create_mac(YOSAL_DIGEST_MODE_HMAC_MD5, key, klen);
  Ydigest_update(dig, message, mlen);
  Ydigest_final(dig);
  Ydigest_hex(dig, signature_md5);
  YTEST_EXPECT_STREQ(signature_md5, "ecb1c47e2e1cdb347feffaa2ff3edb15");
  Ydigest_release(dig);

  printf("Test yosal::Ydigest hmac-sha1\n");
  dig = Ydigest_create_mac(YOSAL_DIGEST_MODE_HMAC_SHA1, key, klen);
  Ydigest_update(dig, message, mlen);
  Ydigest_final(dig);
  Ydigest_hex(dig, signature_sha1);
  YTEST_EXPECT_STREQ(signature_sha1, "e01889c43c82b1b1a8bf172b1fd5abb2447e574c");
  Ydigest_release(dig);

  return 0;
}

static int
test_base64()
{
  int i;
  char *b64;
  /* Test vectors from http://tools.ietf.org/html/rfc4648 */
  const char* refb64[] = {
    "",
    "Zg==",
    "Zm8=",
    "Zm9v",
    "Zm9vYg==",
    "Zm9vYmE=",
    "Zm9vYmFy"
  };

  printf("Test yosal::base64\n");

  for (i = 0; i <= 6; i++) {
    b64 = Ybase64_encode("foobar", i);
    if (b64 == NULL) {
    } else {
      YTEST_EXPECT_STREQ(refb64[i], b64);
      Ymem_free(b64);
    }
  }

  printf("Test passed\n");
  return 0;
}

static int
test_array() {
  int i;
  YArray* array = YArray_createLength(32);
  YTEST_EXPECT_EQ(0, YArray_length(array));

  YArray_append(array, "elementA");
  YArray_append(array, "elementB");
  YTEST_EXPECT_EQ(2, YArray_length(array));

  YTEST_EXPECT_STREQ("elementA", YArray_get(array, 0));
  YTEST_EXPECT_STREQ("elementB", YArray_get(array, 1));

  for (i=2; i<36; i++) {
    YArray_append(array, "fill it up");
  }
  YTEST_EXPECT_EQ(i, YArray_length(array));

  YTEST_EXPECT_STREQ("fill it up", YArray_get(array, i-1));

  YArray_append(array, "the end");
  YTEST_EXPECT_STREQ("the end", YArray_get(array, YArray_length(array)-1));

  YArray_release(array);
  return 0;
}

static int
test_yobject() {

  YOSAL_OBJECT_DECLARE(MyStruct)
  YOSAL_OBJECT_BEGIN
    int some;
    long junk;
  YOSAL_OBJECT_END
  YOSAL_OBJECT_EXPORT(MyStruct)

  printf("Test yosal::yobject\n");

  MyStruct *ptr = (MyStruct*) yobject_create(sizeof(MyStruct), Ymem_free);

  YTEST_EXPECT_EQ((yobject*)ptr, yobject_retain((yobject*)ptr));
  YTEST_EXPECT_EQ(YOSAL_OK, yobject_lock((yobject*)ptr));
  YTEST_EXPECT_EQ(YOSAL_OK, yobject_unlock((yobject*)ptr));
  YTEST_EXPECT_EQ(0, yobject_release((yobject*)ptr));

  ptr = (MyStruct*)yobject_create(sizeof(MyStruct), Ymem_free);
  YTEST_EXPECT_EQ((yobject*)ptr, yobject_retain((yobject*)ptr));
  YTEST_EXPECT_EQ((yobject*)ptr, yobject_retain((yobject*)ptr));
  YTEST_EXPECT_EQ((yobject*)ptr, yobject_retain((yobject*)ptr));
  YTEST_EXPECT_EQ(2, yobject_release((yobject*)ptr));
  YTEST_EXPECT_EQ(1, yobject_release((yobject*)ptr));
  YTEST_EXPECT_EQ(0, yobject_release((yobject*)ptr));

  YTEST_EXPECT_ISNULL(yobject_retain(NULL));

  return 1;
}

static int
test_yrandom()
{
  char s[100];
  char olds[100];

  printf("Test yosal::yrandom\n");

  Yrandom_hexstring(s, 15);
  YTEST_EXPECT_EQ(14, strlen(s));
  strncpy(olds, s, sizeof(olds));

  Yrandom_hexstring(s, 37);
  YTEST_EXPECT_EQ(36, strlen(s));
  YTEST_EXPECT_STRNE(olds, s);

  Yrandom_hexstring(s, 100);
  YTEST_EXPECT_EQ(99, strlen(s));

  return 1;
}

int
main(int argc, char *argv[])
{
  if (argc > 1 && strcmp(argv[1], "help") == 0) {
    usage();
    return 0;
  }

  /* Test hashmap */
  test_hashmap();
  /* Test digest */
  test_digest();
  /* Test base64 */
  test_base64();
  /* Test hmac */
  test_hmac();
  /* Test array */
  test_array();
  /* Test refcounter */
  test_yobject();
  /* Test random */
  test_yrandom();

  fclose(stdin);
  fclose(stdout);
  fclose(stderr);

  return 0;
}

