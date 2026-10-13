#include <stdio.h>

#include "error.h"

void invalid_instruction(char *content, instructionT *instruction, size_t line_no)
{
    size_t i;

    printf("Error line %zu: ", line_no);

    for (i = 0; i < instruction->len; i++)
        printf("%c", content[instruction->start + i]);

    printf(" invalid instruction\n");
}

void invalid_operand(char *content, operandT *operand, size_t line_no)
{
    size_t i;

    printf("Error line %zu: ", line_no);

    for (i = 0; i < operand->len; i++)
        printf("%c", content[operand->start + i]);

    printf(" invalid operand\n");
}
