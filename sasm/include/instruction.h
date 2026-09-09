#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stddef.h>

#include "lexer.h"
#include "register.h"

 //Describes what kind of operand was found.
typedef enum{
    OPERAND_REGISTER,
    OPERAND_IMMEDIATE,
    OPERAND_MEMORY,
    OPERAND_LABEL,
    OPERAND_UNKNOWN
} OperandType;

 //Stores the meaning of an operand.
 //operandT stores where the operand is in the source.
 //Operand stores what the operand represents.
 //Examples:
 //    eax -> REGISTER, 0
 //    10  -> IMMEDIATE, 10
typedef struct{
    OperandType type;
    int value;
} Operand;

 //Convert lexical operands into semantic operands.
int classify_operands(char *content, operandT *operands, size_t operand_count, Register *registers, size_t register_count, Operand **classified);

#endif
