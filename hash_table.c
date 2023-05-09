#define HASH_UNION_STR_SIZE 4

#include "hash_table.h"

union HashUnion {
	char str[HASH_UNION_STR_SIZE];
	size_t hash;
};

static size_t hashFunction(const char* key) {
	union HashUnion hashUnion;
	size_t hash = 0;
	size_t lenKey = strlen(key);
	for (size_t i = 0; i < lenKey; i += HASH_UNION_STR_SIZE) {
		for (size_t j = 0; j < HASH_UNION_STR_SIZE && i + j < lenKey; j++)
			hashUnion.str[j] = key[i + j];
		hash += hashUnion.hash;
		hash <<= 1;
	}
	return hash;
}

struct HashTable* createHashTable(const size_t size) {
	struct HashTable* table = (struct HashTable*)malloc(sizeof(struct HashTable));
	if (table) {
		table->nodes = (struct HashNode**)malloc(sizeof(struct HashNode*) * size);
		if (table->nodes) {
			table->size = size;
			for (size_t i = 0; i < size; i++)
				table->nodes[i] = NULL;
		}
		else {
			free(table);
			table = NULL;
		}
	}
	return table;
}

void deleteHashTable(struct HashTable** table) {
	if (*table) {
		if ((*table)->nodes) {
			for (size_t i = 0; i < (*table)->size; i++)
				if ((*table)->nodes[i]) {
					if ((*table)->nodes[i]->value)
						free((*table)->nodes[i]->value);
					free((*table)->nodes[i]);
				}
			free((*table)->nodes);
			free(*table);
			*table = NULL;
		}
	}
}

static struct HashNode* createHashNode(const size_t hash, void* value) {
	struct HashNode* node = (struct HashNode*)malloc(sizeof(struct HashNode));
	if (node) {
		node->hash = hash;
		node->value = value;
	}
	return node;
}

bool insertToHashTable(struct HashTable* table, const char* key, const void* value) {
	if (table) {
		size_t hash = hashFunction(key);
		size_t index = hash % table->size;

		if (table->nodes[index])
			if (table->nodes[index]->hash == hash)
				return false;

		if (!table->nodes[index]) {
			if (table->nodes[index] = createHashNode(hash, value))
				return true;
			return false;
		}

		for (size_t i = index + 1; i <= table->size; i++) {
			if (i == table->size) i = 0;
			if (i == index) return false;

			if (table->nodes[i])
				if (table->nodes[i]->hash == hash)
					return false;

			if (!table->nodes[i]) {
				if (table->nodes[i] = createHashNode(hash, value))
					return true;
				return false;
			}
		}
	}
	return false;
}

void* getFromHashTable(const struct HashTable* table, const char* key) {
	if (table) {
		size_t hash = hashFunction(key);
		size_t index = hash % table->size;

		if (!table->nodes[index]) return 0;

		if (table->nodes[index]->hash == hash)
			return table->nodes[index]->value;

		for (size_t i = index + 1; i <= table->size; i++) {
			if (i == table->size) i = 0;
			if (i == index || !table->nodes[i]) return NULL;
			if (table->nodes[i]->hash == hash) return table->nodes[i]->value;
		}
	}
	return NULL;
}

void printHashTable(const struct HashTable* table) {
	if (table) {
		for (size_t i = 0; i < table->size; i++) {
			printf("%llu ", i);
			if (table->nodes[i])
				printf("hash: %llu pos: %llu value: %p\n", table->nodes[i]->hash, table->nodes[i]->hash % table->size, table->nodes[i]->value);
			else printf("NULL\n");
		}
	}
}