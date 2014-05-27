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

/*
 * A dynamically allocated memory buffer
 */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "yosal/yosal.h"

enum {
  YBUFFER_STATUS_OK = 0,
  YBUFFER_STATUS_CLOSED,
  YBUFFER_STATUS_ERROR
};

struct YbufferStruct
{
  char *data;
  int datalen;
  int dataincr;
  int pos;
  int status;
};

Ybuffer*
Ybuffer_init(int initiallength)
{
  char *data;
  Ybuffer *membuf;

  membuf = (Ybuffer*) Ymem_malloc(sizeof(Ybuffer));
  if (membuf == NULL) {
    return NULL;
  }

  if (initiallength <= 0) {
    membuf->data = NULL;
    membuf->datalen = 0;
    membuf->dataincr = 64;
  } else {
      data = (char*) Ymem_malloc(initiallength);
    if (data == NULL) {
      Ymem_free(membuf);
      return NULL;
    }
    membuf->data = data;
    membuf->datalen = initiallength;
    membuf->dataincr = initiallength;
  }

  membuf->pos = 0;
  membuf->status = YBUFFER_STATUS_OK;

  return membuf;
}

char*
Ybuffer_detach(Ybuffer *stream, int *datalen)
{
  char *data;

  if (stream == NULL) {
    data = NULL;
    if (datalen != NULL) {
      *datalen = 0;
    }
  } else {
    data = stream->data;
    if (datalen != NULL) {
      *datalen = stream->pos;
    }
    Ymem_free(stream);
  }

  return data;
}

void
Ybuffer_fini(Ybuffer *stream)
{
  char *data;

  data = Ybuffer_detach(stream, NULL);
  if (data != NULL) {
    Ymem_free(data);
  }

}

int
Ybuffer_append(Ybuffer *stream, const char *buf, int buflen)
{
  if (stream == NULL) {
    return -1;
  }
  if (stream->status != YBUFFER_STATUS_OK) {
    return -1;
  }

  if (buf == NULL || buflen == 0) {
    return 0;
  }

  if (buflen < 0) {
    buflen = strlen(buf);
  }

  if (stream->pos + buflen >= stream->datalen - 1) {
    /* Re-allocate larger buffer */
    char *newbuf;
    int newlen;

    newlen = stream->datalen + stream->dataincr;
    if (stream->pos + buflen >= newlen - 1) {
      newlen = stream->pos + buflen + 1;
    }

    newbuf = (char*) Ymem_realloc(stream->data, newlen);
    if (newbuf == NULL) {
	/*
	 * If failed to allocate larger buffer, abort.
	 * Older buffer is still valid
	 */
      stream->status = YBUFFER_STATUS_ERROR;
      return -1;
    }

    stream->data = newbuf;
    stream->datalen = newlen;
  }

  memcpy(stream->data + stream->pos, buf, buflen);
  stream->pos += buflen;
  stream->data[stream->pos] = '\0';

  return buflen;
}

int
Ybuffer_append_format(Ybuffer *stream, const char *format, ...)
{
  va_list ap1;
  va_list ap2;
  int rc;
  int space;

  if (stream == NULL) {
    return -1;
  }
  if (stream->status != YBUFFER_STATUS_OK) {
    return -1;
  }

  va_start(ap1, format);
  va_copy(ap2, ap1);
  space = stream->datalen - stream->pos;
  rc = vsnprintf(stream->data + stream->pos, space, format, ap1);
  if (rc >= space) {
    /* Re-allocate larger buffer */
    char *newbuf;
    int newlen;

    newlen = stream->datalen + stream->dataincr;
    if (stream->pos + rc >= newlen - 1) {
      newlen = stream->pos + rc + 1;
    }

    newbuf = (char*) Ymem_realloc(stream->data, newlen);
    if (newbuf == NULL) {
      // Older buffer is still valid.
      stream->status = YBUFFER_STATUS_ERROR;
      rc = -1;
    } else {
      stream->data = newbuf;
      stream->datalen = newlen;

      space = stream->datalen - stream->pos;
      rc = vsnprintf(stream->data + stream->pos, space, format, ap2);
      if (rc >= space) {
        // Older buffer is still valid.
        stream->status = YBUFFER_STATUS_ERROR;
        rc = -1;
      }
    }
  }
  va_end(ap2);
  va_end(ap1);

  if (rc > 0) {
    stream->pos += rc;
  }

  return rc;
}

int
Ybuffer_append_integer(Ybuffer *stream, int value)
{
    /* Buffer large enough to contain INT_MAX */
    char buf[32];
    int buflen;

    buflen = snprintf(buf, sizeof(buf), "%d", value);
    if (buflen >= sizeof(buf)) {
	/* Buffer too short for integer value (should never happen) */
	return -1;
    }
    return Ybuffer_append(stream, buf, buflen);
}
