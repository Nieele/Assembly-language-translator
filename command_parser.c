#define BUFFER_SIZE 256u
#define OPERATORS_COUNT 6u

#ifdef __unix__
#define strtok_s strtok_r
#endif

#include "command_parser.h"

static bool isBufferOverflow(const char buffer[])
{
	return strlen(buffer) >= BUFFER_SIZE;
}

static void clearToNextLine(FILE* const inFile)
{
	while (fgetc(inFile) != '\n' && !feof(inFile));
}

static void replaceLastCharacter(char buffer[])
{
	buffer[strlen(buffer) - (size_t)((buffer[strlen(buffer) - 1] == '\n') ? 1 : 0)] = '\0';
}

static bool readLineToBuffer(FILE* const inFile, char buffer[])
{
	if (!fgets(buffer, BUFFER_SIZE, inFile))
		return false;
	if (isBufferOverflow(buffer))
		clearToNextLine(inFile);
	replaceLastCharacter(buffer);
	return true;
}

static bool isToken_comment(char* context, char* token, char** pos_comment)
{
	if (token[0] == '/') {
		if (strtok_s(NULL, "", &context))
			token[strlen(token)] = ' ';
		*pos_comment = token + 1;
		return true;
	}
	return false;
}

static bool isToken_label(char* token, char** pos_label, char** pos_operator)
{
	if (*pos_operator)
		return false;
	if (!(*pos_label))
		if (token[strlen(token) - 1] == ':') {
			token[strlen(token) - 1] = '\0';
			*pos_label = token;
			return true;
		}
	if (*pos_label)
		return false;
	return false;
}

static bool isToken_operator(char* token, char** pos_operator)
{
	if (!(*pos_operator)) {
		*pos_operator = token;
		return true;
	}
	return false;
}

static bool isToken_operand(char* token, char** pos_operand)
{
	if (!(*pos_operand)) {
		*pos_operand = token;
		return true;
	}
	return false;
}

static void getTokensFromBuffer(char buffer[], struct StrCommand* strCommand)
{
	strCommand->_label = strCommand->_operator = strCommand->_operand = strCommand->_comment = NULL;
	char* token;
	char* context = NULL;

	token = strtok_s(buffer, " \t", &context);
	if (token != NULL) {
		do {
			if (isToken_comment(context, token, &strCommand->_comment)) break;
			if (isToken_label(token, &strCommand->_label, &strCommand->_operator)) continue;
			// if (isToken_label(token, &strCommand->_label, &strCommand->_operator));
			if (isToken_operator(token, &strCommand->_operator));
			else if (isToken_operand(token, &strCommand->_operand));
		} while (token = strtok_s(NULL, " \t", &context));
	}
}

void printCommandToFile(FILE* const outFile, const struct StrCommand* strCommand)
{
	if (strCommand->_label || strCommand->_operator || strCommand->_operand || strCommand->_comment)
		fprintf(outFile, "Metka: %s Operator: %s Operand: %s Comment: %s\n",
			strCommand->_label, strCommand->_operator, strCommand->_operand, strCommand->_comment);
}

void printCommandToCmd(const struct StrCommand* strCommand) {
	if (strCommand->_label || strCommand->_operator || strCommand->_operand || strCommand->_comment)
		printf("Metka: %s Operator: %s Operand: %s Comment: %s\n",
			strCommand->_label, strCommand->_operator, strCommand->_operand, strCommand->_comment);
}

bool getCommandFromFile(const FILE* const inFile, struct StrCommand* strCommand)
{
	static char buffer[BUFFER_SIZE];

	if (!readLineToBuffer(inFile, buffer))
		return false;
	getTokensFromBuffer(buffer, strCommand);
	return true;
}