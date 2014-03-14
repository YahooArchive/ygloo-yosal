#ifndef _YOSAL_HASH_H
#define _YOSAL_HASH_H 1

#ifdef __cplusplus
extern "C" {
#endif

uint32_t
hash_lookup3(const void *key, size_t length);

uint32_t
hash_fnv1(const void *key, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* _YOSAL_HASH_H */
