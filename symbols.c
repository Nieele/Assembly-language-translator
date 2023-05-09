#include "symbols.h"

struct SymbolsNode* makeSymbolNode(const char* label, const bool type, const uint16_t address) {
	struct SymbolsNode* temp = malloc(sizeof(struct SymbolsNode));
	if (!temp) return NULL;

	strcpy_s(temp->label, SIZE_SYMBOL_NODE_STR, label);
	// temp->addressingMethod = addressingMethod;
	temp->type = type;
	temp->address = address;

	return temp;
}

void insertToSymbolsTable(struct HashTable* symbolsTable, struct SymbolsNode* node) {
	if (!(symbolsTable && node)) {
		perror("Warning! error when inserting a symbol into a SymbolTable: the table or node is zero");
		return;
	}
	if (!insertToHashTable(symbolsTable, node->label, node)) free(node);
}

struct SymbolsNode* getSymbolsNode(const struct HashTable* symbolsTable, const char* key) {
	return (struct SymbolsNode*)getFromHashTable(symbolsTable, key);
}

bool changeAddressSymbol(struct HashTable* symbolTable, const char* key, const uint16_t newAddress) {
	struct SymbolsNode* node = getSymbolsNode(symbolTable, key);
	if (!node) return false;
	node->address = newAddress;
	return true;
}