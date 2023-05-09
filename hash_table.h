#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

struct HashNode {
	size_t hash;
	void* value;
};

struct HashTable {
	struct HashNode** nodes;
	size_t size;
};

static size_t hashFunction(const char* key);
struct HashTable* createHashTable(const size_t size);
void deleteHashTable(struct HashTable** table);
static struct HashNode* createHashNode(const size_t hash, const void* value);
bool insertToHashTable(struct HashTable* table, const char* key, const void* value);
void* getFromHashTable(const struct HashTable* table, const char* key);
void printHashTable(const struct HashTable* table);

#endif HASH_TABLE_H // !HASH_TABLE_H