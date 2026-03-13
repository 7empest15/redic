#include "cache.h"
#include <stdlib.h>

t_cache *cache_create(size_t size)
{
    t_cache *cache = malloc(sizeof(t_cache));
    if (!cache) {
        return NULL;
    }
    cache->buckets = calloc(size, sizeof(t_entry *));
    if (!cache->buckets) {
        free(cache);
        return NULL;
    }
    cache->size = size;
    cache->count = 0;
    return cache;
}

int cache_set(t_cache *cache, const char *key, const char *value)
{
    unsigned long index = hash(key) % cache->size;
    t_entry *entry = cache->buckets[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            free(entry->value);
            entry->value = strdup(value);
            return 0;
        }
        entry = entry->next;
    }

    t_entry *new_entry = malloc(sizeof(t_entry));
    if (!new_entry) {
        return -1;
    }
    new_entry->key = strdup(key);
    new_entry->value = strdup(value);
    new_entry->next = cache->buckets[index];
    cache->buckets[index] = new_entry;
    cache->count++;

    return 0;
}

char *cache_get(t_cache *cache, const char *key)
{
    unsigned long index = hash(key) % cache->size;
    t_entry *entry = cache->buckets[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

void cache_free(t_cache *cache)
{
    for (size_t i = 0; i < cache->size; i++) {
        t_entry *entry = cache->buckets[i];
        while (entry) {
            t_entry *next = entry->next;
            free(entry->key);
            free(entry->value);
            free(entry);
            entry = next;
        }
    }
    free(cache->buckets);
    free(cache);
}