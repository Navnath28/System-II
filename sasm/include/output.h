#ifndef OUTPUT_H
#define OUTPUT_H

#include <stddef.h>

#include "lexer.h"
#include "instruction.h"
#include "register.h"

void display_result(char *content, contentT *contents, instructionT *instructions, size_t instruction_count, operandT **operands, size_t *operand_count, Register *registers, size_t register_count);

#endif
