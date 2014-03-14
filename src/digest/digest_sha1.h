/*	$NetBSD: sha1.h,v 1.2 2002/12/21 04:06:15 schmonz Exp $	*/

/*
 * SHA-1 in C
 * By Steve Reid <steve@edmweb.com>
 * 100% Public Domain
 */

#ifndef _CRYPT_SHA1_H_
#define	_CRYPT_SHA1_H_

#include "digest_types.h"

#define SHA1Init YPRIVATE(SHA1Init)
#define SHA1Update YPRIVATE(SHA1Update)
#define SHA1Final YPRIVATE(SHA1Final)

#ifdef __cplusplus
extern "C" {
#endif

typedef UINT4  y_uint32_t;
typedef unsigned char y_uchar;

typedef struct {
	y_uint32_t state[5];
	y_uint32_t count[2];
	y_uchar    buffer[64];
} SHA1_CTX;

void SHA1Init(SHA1_CTX *context);
void SHA1Update(SHA1_CTX *context, const y_uchar *data, y_uint32_t len);
void SHA1Final(y_uchar digest[20], SHA1_CTX *context);

#undef u_int32_t
#define u_int32_t y_uint32_t

#undef u_char
#define u_char y_uchar

#ifdef __cplusplus
}
#endif

#endif /* _CRYPT_SHA1_H_ */
