#pragma once

#include "list.h"
#define NR_BUCKETS 107

typedef struct Hashmap {
	List * buckets[NR_BUCKETS];
} Hashmap;

int hashmap_new(Hashmap **hashmap);

void hashmap_delete(Hashmap *hashmap);

char *get(Hashmap *hashmap, char *key);

int put(Hashmap *hashmap, char *key, char *value);

void delete(Hashmap *hashmap, char *key);
