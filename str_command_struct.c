#include "str_command_struct.h"

struct MultipleOperand* createMultipleOperand() {
	struct MultipleOperand* multiple = malloc(sizeof(struct MultipleOperand));
	if (!multiple) return NULL;

	multiple->sizeBufferOperand0 = INITIAL_SIZE_BUFFER_OPERAND;
	multiple->sizeBufferOperand1 = INITIAL_SIZE_BUFFER_OPERAND;
	multiple->bufferOperand[0] = malloc(sizeof(char) * multiple->sizeBufferOperand0);
	multiple->bufferOperand[1] = malloc(sizeof(char) * multiple->sizeBufferOperand1);
	if (!(multiple->bufferOperand[0] && multiple->bufferOperand[1])) {
		if (!multiple->bufferOperand[0]) free(multiple->bufferOperand[0]);
		if (!multiple->bufferOperand[1]) free(multiple->bufferOperand[1]);
		return NULL;
	}

	return multiple;
};

void deleteMultipleOperand(struct MultipleOperand** multiple) {
	if (*multiple) {
		if ((*multiple)->bufferOperand[0]) {
			free((*multiple)->bufferOperand[0]);
			(*multiple)->bufferOperand[0] = NULL;
		}
		if ((*multiple)->bufferOperand[1]) {
			free((*multiple)->bufferOperand[1]);
			(*multiple)->bufferOperand[1] = NULL;
		}
		free(*multiple);
		(*multiple) = NULL;
	}
}