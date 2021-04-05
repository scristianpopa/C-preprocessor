#include "hashmap.h"
#define FOUND_KEY 0
#define NO_KEY 1
#define HASH_START 5281

static unsigned long hash(char *str)
{
	unsigned long hash;
	int c;

	hash = HASH_START;
	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;
	return hash;
}

/* used to reallocate in case put uses a key already in the map */
static int resolve_collision(List *node, char *key, char *value)
{
	if (!strcmp(node->key, key)) {
		free(node->val);
		node->val = malloc((strlen(value) + 1) * sizeof(char));
		if (!(node->val)) {
			printf("Key already in map malloc");
			fflush(stdout);
			return -ENOMEM;
		}
		strcpy(node->val, value);
		return FOUND_KEY;
	}
	return NO_KEY;
}

int hashmap_new(Hashmap **hashmap)
{
	int i;

	*hashmap = (Hashmap *)malloc(sizeof(Hashmap));
	if (!(*hashmap)) {
		printf("Hashmap malloc");
		fflush(stdout);
		return -ENOMEM;
	}
	for (i = 0; i < NR_BUCKETS; ++i)
		(*hashmap)->buckets[i] = NULL;
	return 0;
}

void hashmap_delete(Hashmap *hashmap)
{
	int i;
	List *it, *next;

	for (i = 0; i < NR_BUCKETS; ++i) {
		it = hashmap->buckets[i];
		while (it) {
			next = it->next;
			list_delete(it);
			it = next;
		}
	}
	free(hashmap);
}

char *get(Hashmap *hashmap, char *key)
{
	unsigned long hash_value;
	List *it;

	hash_value = hash(key) % NR_BUCKETS;
	it = hashmap->buckets[hash_value];
	while (it) {
		if (!strcmp(it->key, key))
			return it->val;
		it = it->next;
	}
	return NULL;
}

int put(struct Hashmap *hashmap, char *key, char *value)
{
	unsigned long hash_value;
	List *it;
	int code;

	hash_value = hash(key) % NR_BUCKETS;
	it = hashmap->buckets[hash_value];
	if (!it) {
		code = list_new(&hashmap->buckets[hash_value],
				key, value, NULL);
		if (code)
			return code;
	} else {
		while (it->next) {
			code = resolve_collision(it, key, value);
			if (code == -ENOMEM)
				return code;
			if (code == FOUND_KEY)
				return 0;
			it = it->next;
		}

		code = resolve_collision(it, key, value);
		if (code == -ENOMEM)
			return code;
		if (code == FOUND_KEY)
			return 0;

		code = list_new(&it->next, key, value, NULL);
		if (code)
			return code;
	}
	return 0;
}

void delete(Hashmap *hashmap, char *key)
{
	unsigned long hash_value;
	List *it, *next;

	hash_value = hash(key) % NR_BUCKETS;
	it = hashmap->buckets[hash_value];
	if (!it)
		return;

	if (!strcmp(it->key, key)) {
		next = it->next;
		list_delete(it);
		hashmap->buckets[hash_value] = next;
	} else
		while (it->next) {
			if (!strcmp(it->next->key, key)) {
				next = it->next->next;
				list_delete(it->next);
				it->next = next;
				return;
			}
			it = it->next;
		}
}
