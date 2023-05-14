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

static size_t placementPointer;
static size_t currentPosition;
static size_t sizeProgram;
static size_t countLinesCode;

static bool beginFind;
static bool endFind;
static bool interruptionFind;

static size_t getCountLines(FILE* file);
static bool findBeginOperator(const FILE* inFile, const struct HashTable* mnemonicsTable, const struct StrCommand* strCommand);
static bool checkDublicateStructuralOperators(const struct StrCommand* strCommand);
static bool isCommentCommand(const struct StrCommand* strCommand);
static size_t findPosSeparatorInOperand(const struct StrCommand* strCommand);
static void splitOperand(const struct StrCommand* strCommand, struct MultipleOperand* operands, const size_t separatorPosition);
static void insertOperandIntoSymbolsTable(struct HashTable* symbolsTable, const struct StrCommand* strCommand);
static void insertLabelIntoSymbolsTable(struct HashTable* symbolsTable, const struct StrCommand* strCommand);
static bool firstPass_analyzeCodeSegment(const FILE* inFile, struct HashTable* symbolsTable, const struct HashTable* mnemonicsTable);
static bool firstPass_analyzeDataSegment(const FILE* inFile, struct HashTable* symbolsTable, const struct HashTable* mnemonicsTable);
static bool firstPass(const FILE* inFile, struct HashTable* symbolsTable, const struct HashTable* mnemonicsTable);
static uint32_t getOperatorInstruction(const struct HashTable* mnemonicsTable, const struct StrCommand* strCommand);
static uint32_t getOperandInstruction(const struct HashTable* symbolsTable, const struct StrCommand* strCommand);
static uint32_t getMachineInstruction(const struct HashTable* symbolsTable, const struct HashTable* mnemonicsTable, const struct StrCommand* strCommand);
static void secondPass(const FILE* inFile, const FILE* outFile, struct HashTable* symbolsTable, const struct HashTable* mnemonicsTable);

void translateCode(const FILE* inFile, FILE* outFile);

#endif // !TRANSLATOR_H