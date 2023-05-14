#ifndef STR_COMMAND_STRUCT_H
#define STR_COMMAND_STRUCT_H

#define MAX_SIZE_MULTIPLE_OPERAND 32u

#include <malloc.h>
#include <stdio.h>

typedef struct StrCommand {
	char* _label;
	char* _operator;
	char* _operand;
	char* _comment;
};

typedef struct MultipleOperand {
	char operand0[MAX_SIZE_MULTIPLE_OPERAND];
	char operand1[MAX_SIZE_MULTIPLE_OPERAND];
};

#endif // !STR_COMMAND_STRUCT_H