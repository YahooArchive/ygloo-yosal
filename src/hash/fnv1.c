#include "yosal/yosal.h"

uint32_t
hash_fnv1(const void *key, size_t length)
{
    uint32_t h;
    int i;
    unsigned char* data = (unsigned char*) key;

    h = (uint32_t) length;
    for (i = 0; i < length; i++) {
        h = h * 31 + data[i];
    }

    return h;
}
