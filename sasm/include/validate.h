#ifndef VALIDATED_H
#define VALIDATED_H

#include <stddef.h>

#include "lexer.h"
#include "instruction.h"
#include "opcode.h"
#include "register.h"

int validate_instructions(char *content, contentT *contents, instructionT *instructions, size_t instruction_count, operandT **operands, size_t *operand_count, Opcode *opcodes, size_t opcode_count, Register *registers, size_t register_count);

#endif
