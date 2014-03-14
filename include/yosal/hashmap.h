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

#ifndef _YOSAL_HASHMAP_H
#define _YOSAL_HASHMAP_H 1

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Reference to a Yhashmap instance
 */
typedef struct YhashmapStruct Yhashmap;

/**
 * A YhashmapEntry is a key value pair stored in a Yhashmap.
 */
typedef struct YhashmapEntryStruct YhashmapEntry;

/**
 * @see YhashmapSearchStruct
 */
typedef struct YhashmapSearchStruct YhashmapSearch;

/**
 * @defgroup Yhashmap
 *
 * @brief Hashmap data structure
 *
 * Portable Hashmap implementation in C that supports arbitrary (void*) keys and
 * values.
 *
 * @{
 */

/**
 * Structure that carries the state when searching a Yhashmap using
 * Yhashmap_first and Yhashmap_next.
 */
struct YhashmapSearchStruct {
    Yhashmap *map;
    YhashmapEntry *entry;
    size_t bucket;
};

/**
 * Instantiate a new Yhashmap instance. A Yhashmap grows dynamically and can
 * grow beyond its initial capacity. Every Yhashmap has to be released using
 * Yhashmap_release when it is no longer needed.
 *
 * @param initialCapacity of the Yhashmap
 *
 * @return newly created Yhashmap
 */
Yhashmap*
Yhashmap_create(int initialCapacity);

/**
 * Destroy an existing Yhashmap and release associated memory
 *
 * @param hashmap to be released
 *
 * @return YOSAL_OK on success
 */
int
Yhashmap_release(Yhashmap *hashmap);

/**
 * Determine the size of a Yhashamp
 *
 * @param map whose size is needed
 *
 * @return size of map
 */
size_t
Yhashmap_size(Yhashmap* map);

/**
 * Lock a Yhashmap. This function blocks until the lock can be acquired and
 * does not attempt to prevent deadlocks. A Yhashmap never locks or unlocks
 * itself. If a Yhashmap is shared between threads, each thread has to call
 * Yhashmap_lock before interacting with the Yhashmap.
 *
 * @param map to be locked
 *
 * @return YOSAL_OK on success
 */
int
Yhashmap_lock(Yhashmap* map);

/**
 * Unlock a Yhashmap. This will allow blocked calls to Yhashmap_lock to proceed.
 * A Yhashmap can only be unlocked by a thread that has previously called
 * Yhashmap_lock and did not call Yhashmap_unlock yet.
 *
 * @param map to be unlocked
 *
 * @return YOSAL_OK on success
 */
int
Yhashmap_unlock(Yhashmap* map);

/**
 * Insert a new entry into an existing Yhashmap.
 *
 * @param map to insert key to
 * @param key
 * @param keylen
 * @param[out] isNew will be set to YTRUE if a new key has been inserted, YFALSE
 * if an existing key was replaced
 *
 * @return Reference to the newly created entry on success, otherwise NULL
 */
YhashmapEntry*
Yhashmap_put(Yhashmap* map, const void* key, int keylen, YBOOL *isNew);

/**
 * Retreive an entry by looking it up using its key.
 *
 * @param map to perform lookup on
 * @param key
 * @param keylen
 *
 * @return Reference to the retrieved entry on success, otherwise NULL
 */
YhashmapEntry*
Yhashmap_get(Yhashmap* map, const void* key, int keylen);

/**
 * Checks if map contains the given key.
 *
 * @param map to be searched
 * @param key
 * @param keylen
 *
 * @return YTRUE if map contains key, otherwise YFALSE.
 */
YBOOL
Yhashmap_contain(Yhashmap* map, void* key, int keylen);

/**
 * Return the current capacity of the given Yhashmap. The capacity is the number
 * of entries that a Yhashmap can hold before it will resize itself dynamically.
 *
 * @param map
 *
 * @return capacity of map
 */
size_t
Yhashmap_capacity(Yhashmap* map);

/**
 * Determine the number of hash collisions in a Yhashmap. Lower is better.
 *
 * @param map to be checked for collisions
 *
 * @return number of collisions
 */
size_t
Yhashmap_collisions(Yhashmap* map);

/**
 * Obtain refernce to the key of a YhashmapEntry. The caller should not free or modify
 * the obtained key.
 *
 * @param pEntry to obtain key from
 * @param[out] lenptr will contain the length of the returned key
 *
 * @return key of entry
 */
void*
Yhashmap_key(const YhashmapEntry *pEntry, int *lenptr);

/**
 * Obtain refernce to the value of a YhashmapEntry. The caller should not free or modify
 * the obtained value.
 *
 * @param pEntry to obtain key from
 * @param[out] lenptr will contain the length of the returned value
 *
 * @return value of entry
 */
void*
Yhashmap_value(const YhashmapEntry *pEntry, int *lenptr);

/**
 * @brief Set value associated with given hashmap entry
 * @ingroup yosal
 *
 * Set value associated with given hashmap entry
 *
 * @param pEntry hashmap entry, as returned by Yhashmap_get()
 * @param value new value for this entry
 * @param valuelen If zero, store the value as a opaque pointer, without
 *                 allocating further memory
 *                 If negative, value should reference a null terminated
 *                 string. A copy of this string will be saved into hashmap
 *                 If positive, value references a byte array that will be
 *                 copied into hashmap
 *
 * @return previous value in hashmap for this entry, or NULL if the
 *         value was not set.
 */
const void*
Yhashmap_setvalue(YhashmapEntry *pEntry, void *value, int valuelen);

/**
 * Remove an entry from a Yhashamap
 *
 * @param hashmap to remove entry from
 * @param pEntry to be removed
 *
 * @return YOSAL_OK
 */
int
Yhashmap_remove(Yhashmap *hashmap, YhashmapEntry *pEntry);

/**
 * Remove an entry from a Yhashmap using the key as identifier.
 *
 * @param map
 * @param key
 * @param keylen
 *
 * @return reference
 */
const void*
Yhashmap_removekey(Yhashmap* map, void* key, int keylen);

/**
 * Begin iterating over all entries in a hashmap. Provide an allocated sSearch
 * object to this and all subsequent calls to Yhashmap_next.
 *
 * @param hashmap to iterate on
 * @param sSearch
 *
 * @return first entry of hashmap
 */
YhashmapEntry*
Yhashmap_first(Yhashmap *hashmap, YhashmapSearch *sSearch);

/**
 * Retrieve the next entry in a Yhashmap. Call this function after making exactly
 * one call to Yhashmap_first.
 *
 * @param sSearch
 *
 * @return next entry in Yhashmap
 */
YhashmapEntry*
Yhashmap_next(YhashmapSearch *sSearch);

#ifdef __cplusplus
}
#endif

#endif /* _YOSAL_HASHMAP_H */
