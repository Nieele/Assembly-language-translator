#ifndef MNEMONICS_H
#define MNEMONICS_H

#define SIZE_MNEMONIC_NODE_STR 10

#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>
#include "hash_table.h"

typedef struct MnemonicNode {
	char mnemonic[SIZE_MNEMONIC_NODE_STR];
	bool tag; // command = false, directive = true
	uint16_t machineInstruction;
};

// prerequisite: the first 2 directives must designate the beginning and end of the program
static const char* strMnemonics_directive[] = { "begin", "end", "dw", "resw"};

// prerequisite: the first command is an interrupt command that separates the program from the data segment
static const char* strMnemonics_command[] = { "hlt", "add", "addx", "lda", "ldx", "sta"};
static const uint16_t machineInstruction_command[] = { 0x0f, 0x11, 0x12, 0x21, 0x22, 0x31};

static struct MnemonicNode* makeMnemonicNode(const char* mnemonic, const bool tag, const uint16_t machineInstruction);
static void uploadOpMnemonicTable(struct HashTable* mnemonicsTable, const size_t len, const char* op[], const bool tag, const uint16_t instruction[]);
struct HashTable* createMnemonicTable();
struct MnemonicNode* getMnemonicNode(const struct HashTable* mnemonicsTable, const char* key);
bool isCorrectOperator(const struct HashTable* mnemonicTable, const char* str);
bool isBeginOperator(const char* str);
bool isEndOperator(const char* str);
bool isInterruptionOperator(const char* str);
bool isReserveOperator(const char* str);
const char* getBeginOperator();
const char* getEndOperator();
const char* getInterruptionOperator();

#endif // !MNEMONICS_H