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

#define LOG_TAG "yosal:digest"

/**
 * HMAC implementation - This code was originally taken from RFC2104
 * See http://www.ietf.org/rfc/rfc2104.txt and
 * http://www.faqs.org/rfcs/rfc2202.html
 */

#include "yosal/yosal.h"

#include "digest_md5.h"
#include "digest_sha1.h"

#ifdef MAX
#undef MAX
#endif
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define YOSAL_DIGEST_LENGTH MAX(YOSAL_DIGEST_MD5, YOSAL_DIGEST_SHA1)

struct YdigestStruct {
  int mode;
  int finalized;
  int length;
  unsigned char digest[YOSAL_DIGEST_LENGTH];
  uint8_t k_opad[64]; // outer padding of HMAC key

  union {
    MD5_CTX md5;
    SHA1_CTX sha1;
  } engine;
};

static MD5_CTX* MD5Context(Ydigest *digest)
{
  return &(digest->engine.md5);
}

static SHA1_CTX* SHA1Context(Ydigest *digest)
{
  return &(digest->engine.sha1);
}

Ydigest* Ydigest_create_mac(int mode, const char* key, int key_len)
{
  Ydigest* context = Ydigest_create(mode);

  uint8_t k_ipad[64];
  int i;

  memset(k_ipad, 0, sizeof k_ipad);
  memset(context->k_opad, 0, sizeof context->k_opad);
  memcpy(k_ipad, key, key_len);
  memcpy(context->k_opad, key, key_len);

  for (i = 0; i < 64; i++) {
    k_ipad[i] ^= 0x36;
    context->k_opad[i] ^= 0x5c;
  }

  Ydigest_update(context, (char*) k_ipad, 64);

  switch (mode) {
    case YOSAL_DIGEST_MODE_HMAC_MD5:
    case YOSAL_DIGEST_MODE_HMAC_SHA1:
      break;
    default:
      ALOGE("invalid HMAC mode");
      Ymem_free(context);
      return NULL;
  }

  return context;
}

Ydigest* Ydigest_create(int mode)
{
  Ydigest *context;

  context = Ymem_malloc(sizeof (Ydigest));
  if (context == NULL) {
    ALOGE("Failed to allocate Ydigest");
    return NULL;
  }

  if (Ydigest_reset(context, mode) != YOSAL_OK) {
    ALOGE("Failed to reset newly created Ydigest");
    Ymem_free(context);
    return NULL;
  }

  return context;
}

int Ydigest_reset(Ydigest *context, int mode)
{
  switch (mode) {
    case YOSAL_DIGEST_MODE_MD5:
    case YOSAL_DIGEST_MODE_HMAC_MD5:
      MD5Init(MD5Context(context));
      context->length = YOSAL_DIGEST_MD5;
      break;
    case YOSAL_DIGEST_MODE_SHA1:
    case YOSAL_DIGEST_MODE_HMAC_SHA1:
      SHA1Init(SHA1Context(context));
      context->length = YOSAL_DIGEST_SHA1;
      break;
    default:
      ALOGE("can not reset Ydigest with unknown mode");
      return YOSAL_ERROR;
  }

  context->mode = mode;
  context->finalized = 0;

  return YOSAL_OK;
}

int Ydigest_update(Ydigest *context, const char *buf, int buflen)
{
  if (context == NULL) {
    return YOSAL_ERROR;
  }
  if (context->finalized) {
    return YOSAL_ERROR;
  }

  if (buf == NULL) {
    return YOSAL_OK;
  }

  if (buflen < 0) {
    buflen = strlen((char*)buf);
  }

  if (buflen > 0) {
    switch (context->mode) {
      case YOSAL_DIGEST_MODE_MD5:
      case YOSAL_DIGEST_MODE_HMAC_MD5:
        MD5Update(MD5Context(context), (const unsigned char*) buf, buflen);
        break;
      case YOSAL_DIGEST_MODE_SHA1:
      case YOSAL_DIGEST_MODE_HMAC_SHA1:
        SHA1Update(SHA1Context(context), (const unsigned char*) buf, buflen);
        break;
      default:
        return YOSAL_ERROR;
    }
  }

  return YOSAL_OK;
}

int Ydigest_final(Ydigest *context)
{
  MD5_CTX* md5ctx;
  SHA1_CTX* sha1ctx;

  if (context == NULL) {
    return YOSAL_ERROR;
  }

  if (!context->finalized) {
    switch (context->mode) {
      case YOSAL_DIGEST_MODE_MD5:
        MD5Final(&(context->digest[0]), MD5Context(context));
        break;
      case YOSAL_DIGEST_MODE_SHA1:
        SHA1Final(&(context->digest[0]), SHA1Context(context));
        break;

      case YOSAL_DIGEST_MODE_HMAC_MD5:
        md5ctx = MD5Context(context);
        MD5Final(&(context->digest[0]), md5ctx);
        MD5Init(md5ctx);
        MD5Update(md5ctx, context->k_opad, 64);
        MD5Update(md5ctx, &(context->digest[0]), YOSAL_DIGEST_MD5);
        MD5Final(&(context->digest[0]), md5ctx);
        break;
      case YOSAL_DIGEST_MODE_HMAC_SHA1:
        sha1ctx = SHA1Context(context);
        SHA1Final(&(context->digest[0]), sha1ctx);
        SHA1Init(sha1ctx);
        SHA1Update(sha1ctx, context->k_opad, 64);
        SHA1Update(sha1ctx, &(context->digest[0]), YOSAL_DIGEST_SHA1);
        SHA1Final(&(context->digest[0]), sha1ctx);
        break;
      default:
        return YOSAL_ERROR;
    }
    context->finalized = 1;
  }

  return YOSAL_OK;
}

int Ydigest_digest(Ydigest *context, void *buf)
{
  if (context == NULL) {
    return 0;
  }

  Ydigest_final(context);
  if (buf != NULL && context->length > 0) {
    memcpy(buf, context->digest, context->length);
  }

  return context->length;
}

int Ydigest_hex(Ydigest *context, char *out)
{
  int i;
  int nibble;
  int len;
  int hmaclen;

  if (context == NULL) {
    return 0;
  }

  Ydigest_final(context);

  len = context->length;
  if (len <= 0) {
    hmaclen = 0;
  } else {
    hmaclen = 2 * len + 1;
    if (out != NULL) {
      for (i = 0; i < len; i++) {
        nibble = (context->digest[i] >> 4) & 0xf;
        out[2 * i] = (nibble <= 9 ? '0' + nibble : 'a' + (nibble - 10));
        nibble = context->digest[i] & 0xf;
        out[2 * i + 1] = (nibble <= 9 ? '0' + nibble : 'a' + (nibble - 10));
      }
      out[2 * i] = '\0';
    }
  }

  return hmaclen;
}

int Ydigest_release(Ydigest *digest)
{
  if (digest != NULL) {
    Ymem_free(digest);
  }

  return YOSAL_OK;
}
