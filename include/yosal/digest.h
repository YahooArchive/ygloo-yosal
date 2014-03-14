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

#ifndef _YOSAL_DIGEST_H
#define _YOSAL_DIGEST_H 1

#define YOSAL_DIGEST_MODE_UNKNOWN 0
#define YOSAL_DIGEST_MODE_MD5 1
#define YOSAL_DIGEST_MODE_SHA1 2

#define YOSAL_DIGEST_MODE_HMAC_MD5 11
#define YOSAL_DIGEST_MODE_HMAC_SHA1 12

/* hash lengths */
#define YOSAL_DIGEST_MD5 16
#define YOSAL_DIGEST_SHA1 20

#define YOSAL_DIGEST_HEX(l) ((l)*2+1)
#define YOSAL_DIGEST_MD5_HEX YOSAL_DIGEST_HEX(YOSAL_DIGEST_MD5)
#define YOSAL_DIGEST_SHA1_HEX YOSAL_DIGEST_HEX(YOSAL_DIGEST_SHA1)

#ifdef __cplusplus
extern "C" {
#endif

typedef struct YdigestStruct Ydigest;

/**
 * @defgroup Ydigest
 *
 * @brief SHA1 and MD5 support
 *
 * If in doubt, use SHA1.
 *
 * @{
 */


/**
 * Create a new Ydigest instance to compute hashes. The new Ydigest instance has
 * to be released using Ydigest_release when it is no longer needed.
 *
 * @param mode YOSAL_DIGEST_MODE_MD5 or YOSAL_DIGEST_MODE_SHA1
 *
 * @return new Ydigest instance
 */
Ydigest* Ydigest_create(int mode);

/**
 * Create a new Ydigest instance to compute message authentication codes (MAC).
 * The new Ydigest instance has to be released using Ydigest when it is no
 * longer needed.
 *
 * @param mode YOSAL_DIGEST_MODE_HMAC_MD5 or YOSAL_DIGEST_MODE_HMAC_SHA1
 * @param key secret key to be used when computing the MAC
 * @param key_len length of key
 *
 * @return new Ydigest instance
 */
Ydigest* Ydigest_create_mac(int mode, const char* key, int key_len);

/**
 * Reset an existing Ydigest instance. Call this method before reusing an existing
 * instance, even if Ydigest_final has been called before. While you can switch
 * modes between MD5 and SHA1 you can't switch between MAC and non-MAC modes after
 * a Ydigest instance has been created.
 *
 * @param context Ydigest instance to reset
 * @param mode new mode
 *
 * @return YOSAL_OK on success
 */
int Ydigest_reset(Ydigest *context, int mode);

/**
 * Append the given buffer buf to the digest. For example when computing the SHA1
 * sum of the string "foobar", one could call Ydigest update twice once for "foo"
 * and a second time for "bar, followed by a call to Ydigest_final.
 *
 * @param digest existing Ydigest instance
 * @param buf buffer to append to digest input
 * @param buflen length of buffer
 *
 * @return YOSAL_OK on success
 */
int Ydigest_update(Ydigest *digest, const char *buf, int buflen);

/**
 * Compute HASH/MAC after all input has been provided using Ydigest_update.
 *
 * @param digest to be finalized
 *
 * @return YOSAL_OK on success
 */
int Ydigest_final(Ydigest *digest);

/**
 * Retreive digest as binary buffer. outbuf has to point to allocated memory
 * of size YOSAL_DIGEST_MD5 or YOSAL_DIGEST_SHA1.
 *
 * @param ctx Ydigest instance
 * @param[out] outbuf output buffer
 *
 * @return YOSAL_OK on success
 */
int Ydigest_digest(Ydigest *ctx, void *outbuf);

/**
 * Retrieve digest as ASCII encoded HEX string. outbuf has to point to allocated
 * memory of size YOSAL_DIGEST_MD5_HEX or YOSAL_DIGEST_SHA1_HEX.
 *
 * @param ctx Ydigest instance
 * @param[out] outbuf output buffer
 *
 * @return YOSAL_OK on success
 */
int Ydigest_hex(Ydigest *ctx, char *outbuf);

/**
 * Clean up related memory and release the given Ydigest instance.
 *
 * @param digest instance to be terminated
 *
 * @return YOSAL_OK on success
 */
int Ydigest_release(Ydigest *digest);

#ifdef __cplusplus
}
#endif

#endif /* _YOSAL_DIGEST_H */
