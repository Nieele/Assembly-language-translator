#ifndef STR_COMMAND_STRUCT_H
#define STR_COMMAND_STRUCT_H

#define INITIAL_SIZE_BUFFER_OPERAND 32u

#include <malloc.h>
#include <stdio.h>

typedef struct StrCommand {
	char* _label;
	char* _operator;
	char* _operand;
	char* _comment;
};

typedef struct MultipleOperand {
	size_t sizeBufferOperand0;
	size_t sizeBufferOperand1;
	char* bufferOperand[2];
};

struct MultipleOperand* createMultipleOperand();
void deleteMultipleOperand(struct MultipleOperand** multiple);

#endif // !STR_COMMAND_STRUCT_H