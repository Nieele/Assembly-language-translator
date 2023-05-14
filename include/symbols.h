#ifndef SYMBOLS_H
#define SYMBOLS_H

#define SIZE_SYMBOL_NODE_STR 20

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "hash_table.h"

typedef struct SymbolsNode {
	char label[SIZE_SYMBOL_NODE_STR];
	// bool addressingMethod;	// direct = false, index = true
	bool type;				// symbol = false, label = true
	uint16_t address;
};

struct SymbolsNode* makeSymbolNode(const char* label, const bool type, const uint16_t address);
void insertToSymbolsTable(struct HashTable* symbolsTable, struct SymbolsNode* node);
struct SymbolsNode* getSymbolsNode(const struct HashTable* symbolsTable, const char* key);
bool changeAddressSymbol(struct HashTable* symbolTable, const char* key, const uint16_t newAddress);

#endif // !SYMBOLS_H