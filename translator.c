#include "translator.h"

static size_t getCountLines(FILE* file) {
	fseek(file, 0, SEEK_SET);
	char c;
	size_t count = 0;
	while ((c = fgetc(file)) != EOF)
		if (c == '\n')
			count++;
	fseek(file, 0, SEEK_SET);
	return count;
}

// search for the "start" operator, then if the program start address is specified, change the placeCounter
static bool findBeginOperator(const struct StrCommand* strCommand) {
	if (isBeginOperator(strCommand->_operator)) {
		if (strCommand->_operand)
			placementPointer = currentPosition = strtol(strCommand->_operand, NULL, 16); // guaranteed that the address is written correctly
		return true;
	}
	return false;
}

// is there a separator "," in the operator
static int checkSeparatorOperand(const char* str) {
	size_t len = strlen(str);
	for (size_t i = 0; i < len; ++i)
		if (str[i] == ',')
			return i;
	return -1;
}

static bool strncpy_withAllocation(char* destination, size_t* size, const char* source, const size_t count) {
	if (size < count + 1) {
		free(destination);
		destination = malloc(sizeof(char) * count * 1.5);
		if (!destination) {
			perror("failed to allocate memory (strncpy_withAllocation)");
			return false;
		}
		(*size) *= 1.5;
	}

	strncpy_s(destination, *size, source, count);
	return true;
}

static void splittingFullOperand(const struct StrCommand* strCommand, struct MultipleOperand* multiple, size_t* posSeparator) {
	if (!(strCommand && multiple && posSeparator)) {
		perror("failed attempt to split an operand - null (splittingFullOperand)");
		return;
	}

	if ((*posSeparator = checkSeparatorOperand(strCommand->_operand)) != -1) {
		strncpy_withAllocation(multiple->bufferOperand[0], &multiple->sizeBufferOperand0,
			strCommand->_operand, *posSeparator);
		strncpy_withAllocation(multiple->bufferOperand[1], &multiple->sizeBufferOperand1,
			strCommand->_operand + (*posSeparator) + 1, strlen(strCommand->_operand + (*posSeparator)));
	}
	else {
		strncpy_withAllocation(multiple->bufferOperand[0], &multiple->sizeBufferOperand0,
			strCommand->_operand, strlen(strCommand->_operand));
	}
}

// guaranteed that the cursor pointer in the file at the beginning
static bool firstPass(const FILE* inFile, struct HashTable* symbolsTable, struct HashTable* mnemonicTable) {
	placementPointer = 0;
	bool startFind = false;
	bool endFind = false;

	size_t posSeparator = -1; // ","
	struct MultipleOperand* multipleOperand = createMultipleOperand();
	if (!multipleOperand) {
		perror("failed to allocate memory (createMultipleOperand)");
		return false;
	}

	struct StrCommand strCommand;

	// analyze code segment
	while (getCommandFromFile(inFile, &strCommand)) {
		printf("%04x ", currentPosition);
		printCommandToCmd(&strCommand);

		if (!startFind) startFind = findBeginOperator(&strCommand);
		else {
			if (isBeginOperator(strCommand._operator)) {
				perror("Duplicate of the program start keyword found");
				deleteMultipleOperand(&multipleOperand);
				return false;
			}
			if (isEndOperator(strCommand._operator)) {
				endFind = true;
				break;
			}
			if (isInterruptionOperator(strCommand._operator)) {
				currentPosition += MACHINE_COMMAND_SIZE;
				break;
			}

			if (strCommand._label) insertToSymbolsTable(symbolsTable, makeSymbolNode(strCommand._label, true, currentPosition));
			if (strCommand._operator) {
				if (!isCorrectOperator(mnemonicTable, strCommand._operator)) {
					perror("Incorrect operator %s\n", strCommand._operator);
					deleteMultipleOperand(&multipleOperand);
					return false;
				}
			}
			if (strCommand._operand) {
				splittingFullOperand(&strCommand, multipleOperand, &posSeparator);
				if (!isdigit(multipleOperand->bufferOperand[0][0]));
				insertToSymbolsTable(symbolsTable, makeSymbolNode(multipleOperand->bufferOperand[0], true, 0));
			}
			currentPosition += MACHINE_COMMAND_SIZE;
		}
	}


	// analyze data segment
	size_t shift = 1;
	while (getCommandFromFile(inFile, &strCommand)) {
		printf("%04x ", currentPosition);
		printCommandToCmd(&strCommand);
		shift = 1;
		if (isBeginOperator(strCommand._operator)) {
			perror("Duplicate of the program start keyword found");
			deleteMultipleOperand(&multipleOperand);
			return false;
		}
		if (isEndOperator(strCommand._operator)) {
			if (endFind) {
				perror("Duplicate of the program end keyword found");
				deleteMultipleOperand(&multipleOperand);
				return false;
			}
			endFind = true;
		}
		if (isInterruptionOperator(strCommand._operator)) {
			perror("Duplicate of the program interruption keyword found");
			deleteMultipleOperand(&multipleOperand);
			return false;
		}

		if (!strCommand._label && strCommand._operator && strCommand._operand) {
			if (!strcmp(strCommand._operator, "resw"))
				shift = strtol(strCommand._operand, NULL, 10);
			currentPosition += MACHINE_COMMAND_SIZE * shift;
		}
		else if (strCommand._label && strCommand._operator && strCommand._operand) {
			changeAddressSymbol(symbolsTable, strCommand._label, currentPosition);
			currentPosition += MACHINE_COMMAND_SIZE;
		}
	}

	deleteMultipleOperand(&multipleOperand);

	if (!(startFind && endFind)) {
		perror("The beginning or end of the program was not found");
		return false;
	}

	sizeProgram = currentPosition - placementPointer;

	fseek(inFile, 0, SEEK_SET);
	return true;
}

static void secondPass(const FILE* inFile, struct HashTable* symbolsTable, struct HashTable* mnemonicTable) {
	uint32_t machineInstruction;
	struct StrCommand strCommand;

	//skip to the keyword "begin"
	while (getCommandFromFile(inFile, &strCommand))
		if (strCommand._operator)
			if (isBeginOperator(strCommand._operator))
				break;

	bool interruptionFind = false;
	while (getCommandFromFile(inFile, &strCommand)) {
		machineInstruction = 0;

		if (strCommand._operator)
			if (isInterruptionOperator(strCommand._operator)) { // ("hlt")
				interruptionFind = true;
				machineInstruction = getMnemonicNode(mnemonicTable, "hlt")->machineInstruction;
				machineInstruction <<= 16;
			}

		if (!interruptionFind) {
			if (strCommand._operator) {
				struct MnemonicNode* tempMnemo = getMnemonicNode(mnemonicTable, strCommand._operator);
				if (tempMnemo)
					if (!tempMnemo->tag) {
						machineInstruction = tempMnemo->machineInstruction << 16;
						if (strCommand._operand)
							if (!isdigit(strCommand._operand[0]))
								getSymbolsNode(symbolsTable, strCommand._operand)->address;
					}
			}
			if (strCommand._operand) {
				for (size_t i = 0; i < strlen(strCommand._operand); ++i)
					if (strCommand._operand[i] == ',')
						strCommand._operand[i] = '\0';
				struct SymbolsNode* tempSymbol = getSymbolsNode(symbolsTable, strCommand._operand);
				if (tempSymbol) {
					machineInstruction += tempSymbol->address;
				}
			}
		}
		printf("%06x\n", machineInstruction);
		if (interruptionFind) break;
	}

	while (getCommandFromFile(inFile, &strCommand)) {
		machineInstruction = 0;

		if (strCommand._operator) {
			if (!strcmp(strCommand._operator, "dw")) {
				if (strCommand._operand)
					machineInstruction = strtol(strCommand._operand, NULL, 10);
				printf("%06x\n", machineInstruction);
			}
			else if (!strcmp(strCommand._operator, "resw")) {
				printf("%06x\n", machineInstruction);
			}
			else if (isEndOperator(strCommand._operator)) {
				inputPointer = getSymbolsNode(symbolsTable, strCommand._operand)->address;
				break;
			}
		}
	}

	printf("Placement pointer: %x Size of programm: %x Input pointer: %x", placementPointer, sizeProgram, inputPointer);
}

void translateCode(const FILE* inFile, FILE* outFile) {
	struct HashTable* symbolsTable = createHashTable(getCountLines(inFile) * 2);
	struct HashTable* mnemonicsTable = createMnemonicTable();

	if (symbolsTable && mnemonicsTable) {
		if (firstPass(inFile, symbolsTable, mnemonicsTable));
		secondPass(inFile, symbolsTable, mnemonicsTable);
	}

	deleteHashTable(&symbolsTable);
	deleteHashTable(&mnemonicsTable);
}