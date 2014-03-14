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

#include "cencode.h"
#include "cdecode.h"

char*
Ybase64_encode(const char *buf, int buflen)
{
  base64_encodestate bstate;
  const char *ibuf;
  int ilen, olen;
  char *result;
  char obuf[20];
  Ybuffer *ybuf;

  if (buf == NULL) {
    return NULL;
  }

  if (buflen < 0) {
    buflen = strlen(buf);
  }

  /* Output should be (buflen / 3) * 4 + 4 + 1 */
  ybuf = Ybuffer_init(16);
  if (ybuf == NULL) {
    return NULL;
  }

  base64_init_encodestate(&bstate);

  ibuf = buf;
  while (buflen > 0) {
    if (buflen < 12) {
      ilen = buflen;
    } else {
      ilen = 12;
    }

    olen = base64_encode_block(ibuf, ilen, obuf, &bstate);
    Ybuffer_append(ybuf, obuf, olen);

    ibuf += ilen;
    buflen -= ilen;
  }

  olen = base64_encode_blockend(obuf, &bstate);
  Ybuffer_append(ybuf, obuf, olen);

  /* Get sure output buffer is a valid null terminated string */
  Ybuffer_append(ybuf, "\0", 1);

  /* Extract string and release Ybuffer container */
  result = Ybuffer_detach(ybuf, NULL);

  return result;
}

