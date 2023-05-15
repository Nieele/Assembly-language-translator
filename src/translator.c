#include "../include/translator.h"

static size_t getCountLines(const FILE* file) {
	fseek(file, 0, SEEK_SET);
	char c;
	size_t count = 0;
	while ((c = fgetc(file)) != EOF)
		if (c == '\n')
			count++;
	fseek(file, 0, SEEK_SET);
	return count;
}

/// <summary>
/// moves the cursor position to the program start keyword
/// </summary>
/// <param name="inFile">input file</param>
/// <param name="mnemonicsTable">mnemonics table (keywords)</param>
/// <param name="strCommand"></param>
/// <returns>the result of finding the beginning of the program</returns>
static bool findBeginOperator(const FILE* inFile, const struct HashTable* mnemonicsTable, const struct StrCommand* strCommand) {
	while (getCommandFromFile(inFile, strCommand))
		if (strCommand->_operator)
			if (isBeginOperator(strCommand->_operator)) {
				if (strCommand->_operand)
					placementPointer = currentPosition = strtol(strCommand->_operand, NULL, 16);
				beginFind = true;
				return true;
			}

	return false;
}

/// <summary>
/// checks the duplication of the key structural words "begin", "end", "hlt" and their presence
/// </summary>
/// <param name="strCommand"></param>
/// <returns>true if duplicates are found</returns>
static bool checkDublicateStructuralOperators(const struct StrCommand* strCommand) {
	if (strCommand->_operator) {
		if (isBeginOperator(strCommand->_operator)) {
			if (beginFind) return true;
		}
		if (isEndOperator(strCommand->_operator)) {
			if (endFind) return true;
			endFind = true;
		}
		if (isInterruptionOperator(strCommand->_operator)) {
			if (interruptionFind) return true;
			interruptionFind = true;
		}
	}
	return false;
}

static bool isCommentCommand(const struct StrCommand* strCommand) {
	if (!strCommand->_label && !strCommand->_operator && !strCommand->_operand && strCommand->_comment)
		return true;
	return false;
}

static long long findPosSeparatorInOperand(const struct StrCommand* strCommand) {
	size_t lenOperand = strlen(strCommand->_operand);
	for (size_t i = 0; i < lenOperand; ++i)
		if (strCommand->_operand[i] == ',')
			return i;
	return -1;
}

static void splitOperand(const struct StrCommand* strCommand, struct MultipleOperand* operands, const size_t separatorPosition) {
	strncpy_s(operands->operand0, MAX_SIZE_MULTIPLE_OPERAND, strCommand->_operand, separatorPosition);
	strncpy_s(operands->operand1, MAX_SIZE_MULTIPLE_OPERAND, strCommand->_operand + separatorPosition + 1, strlen(strCommand->_operand + separatorPosition + 1));
}

/// <summary>
/// inserts an operand (or 2 operands) into the symbol table, provided they are correct
/// </summary>
/// <param name="symbolsTable"></param>
/// <param name="strCommand"></param>
static void insertOperandIntoSymbolsTable(struct HashTable* symbolsTable, const struct StrCommand* strCommand) {
	if (strCommand->_operand) {
		size_t separatorPosition = findPosSeparatorInOperand(strCommand);
		if (separatorPosition != -1) {
			struct MultipleOperand operands;
			splitOperand(strCommand, &operands, separatorPosition);
			if (!isdigit(operands.operand0[0]))
				insertToSymbolsTable(symbolsTable, makeSymbolNode(operands.operand0, false, 0));
			if (!isdigit(operands.operand1[1]));
				/*insertToSymbolsTable(symbolsTable, makeSymbolNode(operands.operand1, false, 0));*/
		}
		else {
			if (!isdigit(strCommand->_operand[0]))
				insertToSymbolsTable(symbolsTable, makeSymbolNode(strCommand->_operand, false, 0));
		}
	}
}

static void insertLabelIntoSymbolsTable(struct HashTable* symbolsTable, const struct StrCommand* strCommand) {
	if (strCommand->_label)
		insertToSymbolsTable(symbolsTable, makeSymbolNode(strCommand->_label, true, currentPosition));
}

/// <summary>
/// the correctness of keywords is checked, symbols are entered into the symbol table
/// </summary>
/// <param name="inFile"></param>
/// <param name="symbolsTable"></param>
/// <param name="mnemonicsTable"></param>
/// <returns>true if executed successfully</returns>
static bool firstPass_analyzeCodeSegment(const FILE* inFile, struct HashTable* symbolsTable, const struct HashTable* mnemonicsTable, struct StrCommand* strCommand) {

	while (getCommandFromFile(inFile, strCommand)) {
		if (!isCommentCommand(strCommand)) {
			// listing output
			printf("%04x ", currentPosition);
			printCommandToCmd(strCommand);

			if (checkDublicateStructuralOperators(strCommand)) return false;
			if (interruptionFind || endFind) return true;

			insertLabelIntoSymbolsTable(symbolsTable, strCommand);
			if (!isCorrectOperator(mnemonicsTable, strCommand->_operator)) return false;
			insertOperandIntoSymbolsTable(symbolsTable, strCommand);

			currentPosition += MACHINE_COMMAND_SIZE;
			countLinesCode++;
		}
	}
	return true;
}

static bool firstPass_analyzeDataSegment(const FILE* inFile, struct HashTable* symbolsTable, const struct HashTable* mnemonicsTable, struct StrCommand* strCommand) {
	size_t shift;
	if (interruptionFind) currentPosition += MACHINE_COMMAND_SIZE;

	while (getCommandFromFile(inFile, strCommand)) {
		if (!isCommentCommand(strCommand)) {
			// listing output
			printf("%04x ", currentPosition);
			printCommandToCmd(strCommand);
			
			if (checkDublicateStructuralOperators(strCommand)) return false;
			if (endFind) return true;

			shift = 1;
			changeAddressSymbol(symbolsTable, strCommand->_label, currentPosition);
			if (isReserveOperator(strCommand->_operator))
				shift = strtol(strCommand->_operand, NULL, 10);

			currentPosition += MACHINE_COMMAND_SIZE * shift;
			countLinesCode++;
		}
	}
	return true;
}

static bool firstPass(const FILE* inFile, struct HashTable* symbolsTable, const struct HashTable* mnemonicsTable) {
	struct StrCommand strCommand;
	beginFind = endFind = interruptionFind = false;

	if (!findBeginOperator(inFile, mnemonicsTable, &strCommand)) return false;
	
	// listing for begin
	printf("%04x ", currentPosition);
	printCommandToCmd(&strCommand);

	if (firstPass_analyzeCodeSegment(inFile, symbolsTable, mnemonicsTable, &strCommand))
		firstPass_analyzeDataSegment(inFile, symbolsTable, mnemonicsTable, &strCommand);
	sizeProgram = currentPosition - placementPointer;

	printf("Placement point: %04xh Size of program: %xh Entry point: %04xh\n", placementPointer, sizeProgram, placementPointer);

	if (beginFind && endFind)
		return true;

	return false;
}

static uint32_t getOperatorInstruction(const struct HashTable* mnemonicsTable, const struct StrCommand* strCommand) {
	struct MnemonicNode* tempMnemo = getMnemonicNode(mnemonicsTable, strCommand->_operator);
	if (!tempMnemo->tag) return tempMnemo->machineInstruction;
	return 0;
}

static uint32_t getOperandInstruction(const struct HashTable* symbolsTable, const struct StrCommand* strCommand) {
	if (strCommand->_operand) {
		size_t separatorPosition = findPosSeparatorInOperand(strCommand);
		struct MultipleOperand operands;
		if (separatorPosition != -1) splitOperand(strCommand, &operands, separatorPosition);
		const char* currentOperand = separatorPosition != -1 ? operands.operand0 : strCommand->_operand;

		struct SymbolsNode* node = getSymbolsNode(symbolsTable, currentOperand);
		if (!isdigit(currentOperand[0])) return node->address;
		else if (!isReserveOperator(strCommand->_operator)) 
			return strtol(currentOperand, NULL, 10);
	}
	return 0;
}

static uint32_t getMachineInstruction(const struct HashTable* symbolsTable, const struct HashTable* mnemonicsTable, const struct StrCommand* strCommand) {
	uint32_t machineInstruction = 0;
	if (strCommand->_operator) {
		machineInstruction += getOperatorInstruction(mnemonicsTable, strCommand);
		machineInstruction <<= 16;
		machineInstruction += getOperandInstruction(symbolsTable, strCommand);
	}
	return machineInstruction;
}

static void secondPass(const FILE* inFile, FILE* outFile, struct HashTable* symbolsTable, const struct HashTable* mnemonicsTable) {
	fseek(inFile, 0, SEEK_SET);
	beginFind = endFind = interruptionFind = false;

	struct StrCommand strCommand;
	uint32_t machineInstruction;
	size_t shift;
	size_t lines = countLinesCode;
	bool firstOutput = true;

	findBeginOperator(inFile, mnemonicsTable, &strCommand);
	fprintf(outFile, ":%02d%04x%04xXX\n", 2, currentPosition, 34);
	while (getCommandFromFile(inFile, &strCommand)) {
		if (!isCommentCommand(&strCommand)) {
			shift = 1;

			checkDublicateStructuralOperators(&strCommand);
			if (endFind) break;

			machineInstruction = getMachineInstruction(symbolsTable, mnemonicsTable, &strCommand);
			printf("%06x ", machineInstruction);

			if (isReserveOperator(strCommand._operator))
				shift = strtol(strCommand._operand, NULL, 10);

			currentPosition += MACHINE_COMMAND_SIZE * shift;

			if (firstOutput) {
				fprintf(outFile, ":%02d%04x%02d%06x", lines != 0 ? 6 : 3, currentPosition, 0, machineInstruction);
				firstOutput = false;
			}
			else {
				fprintf(outFile, "%06xXX\n", machineInstruction);
				firstOutput = true;
			}
			--lines;
		}
	}
	if (!firstOutput) fprintf(outFile, "XX\n");
	fprintf(outFile, ":%02d%04x%02dXX", 0, placementPointer, 1);
}

void translateCode(const FILE* inFile, FILE* outFile) {
	struct HashTable* symbolsTable = createHashTable(getCountLines(inFile) * 2);
	struct HashTable* mnemonicsTable = createMnemonicTable();

	if (symbolsTable && mnemonicsTable) {
		if (firstPass(inFile, symbolsTable, mnemonicsTable));
			secondPass(inFile, outFile, symbolsTable, mnemonicsTable);
	}

	deleteHashTable(&symbolsTable);
	deleteHashTable(&mnemonicsTable);
}