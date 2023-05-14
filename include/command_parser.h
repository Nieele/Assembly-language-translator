#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h> // exit()
#include <stdbool.h> // bool, true, false
#include "str_command_struct.h"

static bool isBufferOverflow(const char buffer[]);
static void clearToNextLine(FILE* const inFile);
static void replaceLastCharacter(char buffer[]);
static bool readLineToBuffer(FILE* const inFile, char buffer[]);
static bool isToken_comment(char* context, char* token, char** pos_comment);
static bool isToken_label(char* token, char** pos_label, char** pos_operator);
static bool isToken_operator(char* token, char** pos_operator);
static bool isToken_operand(char* token, char** pos_operand);
static void getTokensFromBuffer(char buffer[], struct StrCommand* strCommand);
void printCommandToFile(FILE* const outFile, const struct StrCommand* strCommand);
void printCommandToCmd(const struct StrCommand* strCommand);
bool getCommandFromFile(const FILE* const inFile, struct StrCommand* strCommand);

#endif // !COMMAND_PARSER_H