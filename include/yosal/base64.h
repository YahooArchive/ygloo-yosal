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

#ifndef _YOSAL_BASE64_H
#define _YOSAL_BASE64_H 1

/**
 * @defgroup Ybase64
 *
 * @brief base64 support
 *
 * This module provides base 64 encoding support
 *
 * @{
 */

/**
 * Encode given binary buffer using base64. The returned buffer
 * has to be freed by the caller using Ymem_free.
 *
 * @param buf binary buffer to be encoded
 * @param buflen length of binary buffer
 *
 * @return base 64 ASCII string
 */
char*
Ybase64_encode(const char *buf, int buflen);

#endif /* _YOSAL_BASE64_H */
