#include "yosal/yosal.h"

/*
 * A basic and portable implementation for hash map.
 *
 * Partially derived from hashmap implementation from Android
 * Open-Source Project. See libcutils/hashmap.c from
 *    https://android.googlesource.com/platform/system/core.git
 */

/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

#include <pthread.h>

#define HASHSIZE_POW2 1

/*
 Why reinvent the wheel?
 - STL version is not compatible with system where exceptions and
 RTTI are not available. For portability reason, we want to limit
 dependencies to Posix C only
 - Android has a hasmap class into libcutils, but this is highly
 platform specific
 - overall, this is a core yet compact feature, and we need some
 specific features (e.g. high level of control in iterator)

 This implementation assume keys are byte arrays, i.e.
 (const void* bytes, int len)
 Values are reference to anything (i.e. void*). Managing life cycle
 of values is under the responsibility of the caller.
 */

struct YhashmapEntryStruct {
  void* key;
  int keylen;
  void* value;
  int valuelen;
  uint32_t hash;
  YhashmapEntry* next;
};

struct YhashmapStruct {
  YhashmapEntry** buckets;
  size_t bucketCount;
  size_t bucketMax;
  size_t size;
  pthread_mutex_t lock;
};

/* Compute bucket for a given hash */
int
calculateIndex(size_t bucketCount, uint32_t hash)
{
  /* This is equivalent to (hash % bucketCount), but is much
   more efficient on systems where modulo requires multiple
   cpu cycles. This however requires bucketCount to be a
   power of 2 */
#if HASHSIZE_POW2
  return ((size_t) hash) & (bucketCount - 1);
#else
  return (((size_t) hash) % bucketCount);
#endif
}

static void
expandIfNecessary(Yhashmap* map)
{
  if (map->bucketCount >= map->bucketMax) {
    /* Don't allow further expansion */
    return;
  }

  /* If the load factor exceeds 0.75... */
  if (map->size > ((map->bucketCount * 3) / 4)) {
    size_t i;
    size_t newBucketCount = map->bucketCount << 1;
    YhashmapEntry** newBuckets = Ymem_calloc(newBucketCount, sizeof(YhashmapEntry*));
    if (newBuckets == NULL) {
      /* Abort expansion. */
      return;
    }

    /* Move over existing entries. */
    for (i = 0; i < map->bucketCount; i++) {
      YhashmapEntry* entry = map->buckets[i];
      while (entry != NULL) {
        YhashmapEntry* next = entry->next;
        size_t index = calculateIndex(newBucketCount, entry->hash);
        entry->next = newBuckets[index];
        newBuckets[index] = entry;
        entry = next;
      }
    }

    /* Copy over internals. */
    Ymem_free(map->buckets);

    map->buckets = newBuckets;
    map->bucketCount = newBucketCount;
  }
}

static YhashmapEntry*
createEntry(const void* key, int keylen, int hash, int nullterminate)
{
  char *keydup = NULL;
  int keyalloc = 0;

  YhashmapEntry* entry = Ymem_malloc(sizeof(YhashmapEntry));
  if (entry == NULL) {
    return NULL;
  }

  if (keylen > 0) {
    keyalloc = keylen;
    if (nullterminate) {
      keyalloc++;
    }
    keydup = Ymem_malloc(keyalloc);
    if (keydup == NULL) {
      /* Running out of memory, clean up and return failure */
      Ymem_free(entry);
      return NULL;
    }
    memcpy(keydup, key, keylen);
    if (nullterminate) {
      keydup[keylen] = '\0';
    }
  } else {
    keydup = NULL;
    keylen = 0;
  }

  entry->key = keydup;
  entry->keylen = keylen;
  entry->value = NULL;
  entry->valuelen = 0;
  entry->hash = hash;
  entry->next = NULL;

  return entry;
}

static inline int
equalKeys(const void* keyA, int keylenA, int hashA,
          const void* keyB, int keylenB, int hashB)
{
  if (keyA == NULL) {
    keylenA = 0;
  }
  if (keyB == NULL) {
    keylenB = 0;
  }
  if (keylenA == 0 && keylenB == 0) {
    return 1;
  }
  if (keylenA != keylenB || hashA != hashB) {
    return 0;
  }
  if (keyA == keyB) {
    return 1;
  }
  return (memcmp(keyA, keyB, keylenA) == 0);
}


/* Hash function used for hashing the key */
static uint32_t
hashKey(const void* key, int keylen)
{
  return hash_lookup3(key, keylen);
}

/* Public API */
Yhashmap*
Yhashmap_create(int initialCapacity)
{
  size_t i;

  Yhashmap *map = Ymem_malloc(sizeof(struct YhashmapStruct));
  if (map == NULL) {
    return NULL;
  }

  if (initialCapacity < 2) {
    initialCapacity = 2;
  }

  /* 0.75 load factor. */
  size_t minimumBucketCount = initialCapacity * 4 / 3;
  map->bucketCount = 1;
  while (map->bucketCount <= minimumBucketCount) {
    /* Bucket count must be power of 2. */
    map->bucketCount <<= 1;
  }

  /* Set to 0 to prevent automatic expand */
  map->bucketMax = 0;

  /* Number of elements in map */
  map->size = 0;

  map->buckets = Ymem_malloc(map->bucketCount * sizeof(YhashmapEntry*));
  if (map->buckets == NULL) {
    Ymem_free(map);
    return NULL;
  }
  for (i = 0; i < map->bucketCount; i++) {
    map->buckets[i] = NULL;
  }

  pthread_mutex_init(&map->lock, NULL);

  return map;
}

int
Yhashmap_release(Yhashmap *hashmap)
{
  size_t i;

  if (hashmap == NULL) {
    return YOSAL_ERROR;
  }

  for (i = 0; i < hashmap->bucketCount; i++) {
    YhashmapEntry* entry = hashmap->buckets[i];
    while (entry != NULL) {
      YhashmapEntry* next = entry->next;

	    if (entry->keylen > 0 && entry->key != NULL) {
        Ymem_free(entry->key);
	    }
	    if (entry->valuelen > 0 && entry->value != NULL) {
        Ymem_free(entry->value);
	    }

      Ymem_free(entry);
      entry = next;
    }
  }

  Ymem_free(hashmap->buckets);
  pthread_mutex_destroy(&hashmap->lock);
  Ymem_free(hashmap);

  return YOSAL_OK;
}

size_t
Yhashmap_size(Yhashmap* map)
{
  return map->size;
}

int
Yhashmap_lock(Yhashmap* map)
{
  if (pthread_mutex_lock(&map->lock) == 0) {
    return YOSAL_OK;
  } else {
    return YOSAL_ERROR;
  }
}

int
Yhashmap_unlock(Yhashmap* map)
{
  if (pthread_mutex_unlock(&map->lock) == 0) {
    return YOSAL_OK;
  } else {
    return YOSAL_ERROR;
  }
}

YhashmapEntry*
Yhashmap_put(Yhashmap* map,
             const void* key, int keylen,
             YBOOL *isNew)
{
  uint32_t hash;
  size_t index;
  YhashmapEntry** p;
  int nullterminate = 0;

  if (key == NULL) {
    keylen = 0;
  } else if (keylen < 0) {
    /* Input is a null terminated string. Take the whole string, including
	   the null terminator, as key */
    keylen = strlen(key);
    nullterminate = 1;
  }

  hash = hashKey(key, keylen);
  index = calculateIndex(map->bucketCount, hash);
  p = &(map->buckets[index]);

  fflush(stdout);

  while (true) {
    YhashmapEntry *current = *p;

    /* Add a new entry. */
    if (current == NULL) {
      *p = createEntry(key, keylen, hash, nullterminate);
      if (*p == NULL) {
        /* Out of memory */
        errno = ENOMEM;
        return NULL;
      }

      map->size++;
      expandIfNecessary(map);

      if (isNew != NULL) {
        *isNew = 1;
      }
      return *p;
    }

    /* Replace existing entry */
    if (equalKeys(current->key, current->keylen, current->hash,
                  key, keylen, hash)) {
      if (isNew != NULL) {
        *isNew = 0;
      }
      return current;
    }

    /* Move to next entry. */
    p = &current->next;
  }
}

YhashmapEntry*
Yhashmap_get(Yhashmap* map, const void* key, int keylen)
{
  uint32_t hash;
  size_t index;
  YhashmapEntry* entry;

  if (key == NULL) {
    keylen = 0;
  } else if (keylen < 0) {
    keylen = strlen(key);
  }

  hash = hashKey(key, keylen);
  index = calculateIndex(map->bucketCount, hash);
  entry = map->buckets[index];

  while (entry != NULL) {
    if (equalKeys(entry->key, entry->keylen, entry->hash,
                  key, keylen, hash)) {
      return entry;
    }
    entry = entry->next;
  }

  return NULL;
}

YBOOL
Yhashmap_contain(Yhashmap* map, void* key, int keylen)
{
  return (Yhashmap_get(map, key, keylen) != NULL);
}

size_t
Yhashmap_capacity(Yhashmap* map)
{
  size_t bucketCount = map->bucketCount;
  return (bucketCount * 3) / 4;
}

size_t
Yhashmap_collisions(Yhashmap* map)
{
  size_t collisions = 0;
  size_t i;
  for (i = 0; i < map->bucketCount; i++) {
    YhashmapEntry* entry = map->buckets[i];
    while (entry != NULL) {
      if (entry->next != NULL) {
        collisions++;
      }
      entry = entry->next;
    }
  }
  return collisions;
}

void*
Yhashmap_key(const YhashmapEntry *pEntry, int *lenptr)
{
  void *result = NULL;
  int len = 0;

  if (pEntry != NULL) {
    len = pEntry->keylen;
    result = pEntry->key;
  }

  if (lenptr != NULL) {
    *lenptr = len;
  }

  return result;
}

void*
Yhashmap_value(const YhashmapEntry *pEntry, int *lenptr)
{
  void *result = NULL;
  int len = 0;

  if (pEntry != NULL) {
    len = pEntry->valuelen;
    result = pEntry->value;
  }

  if (lenptr != NULL) {
    *lenptr = len;
  }

  return result;
}

const void*
Yhashmap_setvalue(YhashmapEntry *pEntry, void *value, int valuelen)
{
  const void *previousValue = NULL;
  void *valuedup = NULL;

  if (pEntry != NULL) {
    if (valuelen < 0) {
	    valuelen = strlen(value);
    }
    if (valuelen > 0) {
	    valuedup = Ymem_malloc(valuelen);
	    if (valuedup == NULL) {
        /* Out of memory */
        errno = ENOMEM;
        return NULL;
	    }
	    memcpy(valuedup, value, valuelen);
    }

    previousValue = pEntry->value;
    if (pEntry->valuelen > 0 && pEntry->value != NULL) {
	    Ymem_free(pEntry->value);
    }
    pEntry->value = valuedup;
    pEntry->valuelen = valuelen;
  }

  return previousValue;
}

int
Yhashmap_remove(Yhashmap *map, YhashmapEntry *pEntry)
{
  size_t index;
  YhashmapEntry* entry;

  if (pEntry == NULL) {
    return YOSAL_ERROR;
  }

  if (map != NULL) {
    index = calculateIndex(map->bucketCount, pEntry->hash);
    entry = map->buckets[index];
    if (entry == pEntry) {
	    map->buckets[index] = pEntry->next;
      map->size--;
    } else {
	    while (entry != NULL) {
        if (entry->next == pEntry) {
          entry->next = pEntry->next;
          map->size--;
          break;
        }
        entry = entry->next;
	    }
    }
  }

  if (pEntry->keylen > 0 && pEntry->key != NULL) {
    Ymem_free(pEntry->key);
  }
  if (pEntry->valuelen > 0 && pEntry->value != NULL) {
    Ymem_free(pEntry->value);
  }
  Ymem_free(pEntry);

  return YOSAL_OK;
}

const void*
Yhashmap_removekey(Yhashmap* map, void* key, int keylen)
{
  void *value;
  int valuelen;
  YhashmapEntry *entry;

  entry = Yhashmap_get(map, key, keylen);
  if (entry == NULL) {
    return NULL;
  }

  value = Yhashmap_value(entry, &valuelen);
  Yhashmap_remove(map, entry);

  return value;
}

/* Hash iterator without callbacks */
YhashmapEntry*
Yhashmap_first(Yhashmap *map, YhashmapSearch *sSearch)
{
  size_t i;

  /* Find next non-empty bucket */
  for (i = 0; i < map->bucketCount; i++) {
    if (map->buckets[i] != NULL) {
      if (sSearch != NULL) {
        sSearch->map = map;
        sSearch->bucket = i;
        sSearch->entry = map->buckets[i];
      }
      return map->buckets[i];
    }
  }

  return NULL;
}

YhashmapEntry*
Yhashmap_next(YhashmapSearch *sSearch)
{
  size_t i;
  Yhashmap *hashmap;

  if (sSearch == NULL || sSearch->map == NULL || sSearch->entry == NULL) {
    return NULL;
  }

  hashmap = sSearch->map;

  /* Get next entry in current bucket */
  if (sSearch->entry->next) {
    sSearch->entry = sSearch->entry->next;
    return sSearch->entry;
  }

  /* Find next non-empty bucket */
  for (i = sSearch->bucket+1; i < hashmap->bucketCount; i++) {
    if (hashmap->buckets[i] != NULL) {
      sSearch->bucket = i;
      sSearch->entry = hashmap->buckets[i];
      return hashmap->buckets[i];
    }
  }

  /* No entry found */
  sSearch->entry = NULL;
  return NULL;
}
