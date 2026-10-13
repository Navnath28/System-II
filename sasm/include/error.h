#ifndef ERROR_H
#define ERROR_H

#include <stddef.h>

#include "lexer.h"

void invalid_instruction(char *content, instructionT *instruction, size_t line_no);
void invalid_operand(char *content, operandT *operand, size_t line_no);

#endif

