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

#ifndef _YOSAL_YRANDOM_H
#define	_YOSAL_YRANDOM_H

#define YOSAL_NONCE_LENGTH (16+1)

#ifdef	__cplusplus
extern "C" {
#endif


/**
 * @defgroup Yrandom
 *
 * @brief Portable random number generator
 *
 * Supports generation of pseudo random numbers and nonces derived from them.
 *
 * @{
 */

/**
 * Initialize pseudo random number generator.
 * Call this method at least once. Should be called from the "main" thread for
 * best results.
 *
 * @return YOSAL_OK on success
 */
int Yrandom_init();

/**
 * Generate a 32 bits random integer.
 *
 * @return random unsigned 32bits integer
 */
uint32_t Yrandom();

/**
 * Generate a printable random hex string. Randomness might suffer when called
 * from multiple threads at the same time. Outbuf has to reference allocated
 * memory with a size of len or bigger. The generated random string is '\0'
 * terminated.
 *
 * @param[out] nonce buffer to store the new nonce
 *
 * @return YOSAL_OK on success
 */
int Yrandom_hexstring(char* outbuf, int len);

#ifdef	__cplusplus
}
#endif

#endif	/* YRANDOM_H */

