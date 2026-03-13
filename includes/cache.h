#ifndef CACHE_H
#define CACHE_H

#include <stddef.h>
#include <string.h>

#include "hash.h"

typedef struct Entry
{
    char *key;
    char *value;
    struct Entry *next;
} Entry;

typedef struct Cache
{
    Entry **buckets;
    size_t size;
    size_t count;
} Cache;

Cache *cache_create(size_t size);
int cache_set(Cache *cache, const char *key, const char *value);
char *cache_get(Cache *cache, const char *key);
void cache_free(Cache *cache);

#endif // CACHE_H