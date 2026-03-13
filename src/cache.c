#include "cache.h"
#include <stdlib.h>

Cache *cache_create(size_t size)
{
    Cache *cache = malloc(sizeof(Cache));
    if (!cache) {
        return NULL;
    }
    cache->buckets = calloc(size, sizeof(Entry *));
    if (!cache->buckets) {
        free(cache);
        return NULL;
    }
    cache->size = size;
    cache->count = 0;
    return cache;
}

int cache_set(Cache *cache, const char *key, const char *value)
{
    unsigned long index = hash(key) % cache->size;
    Entry *entry = cache->buckets[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            free(entry->value);
            entry->value = strdup(value);
            return 0;
        }
        entry = entry->next;
    }

    Entry *new_entry = malloc(sizeof(Entry));
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

char *cache_get(Cache *cache, const char *key)
{
    unsigned long index = hash(key) % cache->size;
    Entry *entry = cache->buckets[index];
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

void cache_free(Cache *cache)
{
    for (size_t i = 0; i < cache->size; i++) {
        Entry *entry = cache->buckets[i];
        while (entry) {
            Entry *next = entry->next;
            free(entry->key);
            free(entry->value);
            free(entry);
            entry = next;
        }
    }
    free(cache->buckets);
    free(cache);
}