#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#define MACHINE_COMMAND_SIZE 3

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include "command_parser.h"
#include "mnemonics.h"
#include "symbols.h"

static size_t getCountLines(FILE* file);
static bool findBeginOperator(const struct StrCommand* strCommand);
static int checkSeparatorOperand(const char* str);
static bool strncpy_withAllocation(char* destination, size_t* size, const char* source, const size_t count);
static void splittingFullOperand(const struct StrCommand* strCommand, struct MultipleOperand* multiple, size_t* posSeparator);
static bool firstPass(const FILE* inFile, struct HashTable* symbolsTable, struct HashTable* mnemonicTable);
static void secondPass(const FILE* inFile, struct HashTable* symbolsTable, struct HashTable* mnemonicTable);
void translateCode(const FILE* inFile, FILE* outFile);

uint16_t placementPointer;
uint16_t currentPosition;
uint16_t sizeProgram;
uint16_t inputPointer;

#endif // !TRANSLATOR_H