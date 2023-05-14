#ifdef __unix__
#define strtok_s strtok_r
#endif

#include "../include/mnemonics.h"

static struct MnemonicNode* makeMnemonicNode(const char* mnemonic, const bool tag, const uint16_t machineInstruction) {
	struct MnemonicNode* temp = malloc(sizeof(struct MnemonicNode));
	if (!temp) return NULL;

	strcpy_s(temp->mnemonic, SIZE_MNEMONIC_NODE_STR, mnemonic);
	temp->tag = tag;
	temp->machineInstruction = machineInstruction;

	return temp;
}

// guaranteed that the pointer to the mnemonicTable is not null
static void uploadOpMnemonicTable(struct HashTable* mnemonicsTable, const size_t len, const char* op[], const bool tag, const uint16_t instruction[]) {
	struct MnemonicNode* temp;
	for (size_t i = 0; i < len; ++i)
		if (!(temp = makeMnemonicNode(op[i], tag, (instruction ? instruction[i] : NULL))))
			perror("Warning! malloc has not allocated memory (createMnemonicTable)");
		else insertToHashTable(mnemonicsTable, op[i], temp);
}

struct HashTable* createMnemonicTable() {
	size_t len_directive = (sizeof(strMnemonics_directive) / sizeof(strMnemonics_directive[0]));
	size_t len_command = (sizeof(strMnemonics_command) / sizeof(strMnemonics_command[0]));

	// create a hash table with a size 2 times larger than commands and directives
	struct HashTable* mnemonicsTable = createHashTable((len_directive + len_command) * 2);
	if (!mnemonicsTable) return NULL;

	// upload mnemonics of commands to the table
	uploadOpMnemonicTable(mnemonicsTable, len_command, strMnemonics_command, false, machineInstruction_command);
	// upload mnemonics of directives to the table
	uploadOpMnemonicTable(mnemonicsTable, len_directive, strMnemonics_directive, true, NULL);

	return mnemonicsTable;
}

struct MnemonicNode* getMnemonicNode(const struct HashTable* mnemonicsTable, const char* key) {
	return (struct MnemonicNode*)getFromHashTable(mnemonicsTable, key);
}

bool isCorrectOperator(const struct HashTable* mnemonicTable, const char* str) {
	if (!str) return true;
	if (getMnemonicNode(mnemonicTable, str) != NULL) return true;
	return false;
}

bool isBeginOperator(const char* str) {
	if (!str) return false;
	if (!strcmp(str, strMnemonics_directive[0])) return true;
	return false;
}

bool isEndOperator(const char* str) {
	if (!str) return false;
	if (!strcmp(str, strMnemonics_directive[1])) return true;
	return false;
}

bool isInterruptionOperator(const char* str)
{
	if (!str) return false;
	if (!strcmp(str, strMnemonics_command[0])) return true;
	return false;
}

bool isReserveOperator(const char* str) {
	if (!str) return false;
	if (!strcmp(str, "resw")) return true;
	return false;
}

const char* getBeginOperator() {
	return strMnemonics_directive[0];
}

const char* getEndOperator() {
	return strMnemonics_directive[1];
}

const char* getInterruptionOperator() {
	return strMnemonics_command[0];
}