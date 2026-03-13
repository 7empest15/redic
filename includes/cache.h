#ifndef CACHE_H
#define CACHE_H

#include <stddef.h>
#include <string.h>

#include "hash.h"

typedef struct t_entry
{
    char *key;
    char *value;
    struct t_entry *next;
} t_entry;

typedef struct t_cache
{
    t_entry **buckets;
    size_t size;
    size_t count;
} t_cache;

t_cache *cache_create(size_t size);
int cache_set(t_cache *cache, const char *key, const char *value);
char *cache_get(t_cache *cache, const char *key);
void cache_free(t_cache *cache);

#endif // CACHE_H