#include<stdio.h>
#include<stdlib.h>

#include "output.h"


void display_result(char *content, contentT *contents, instructionT *instructions, size_t instruction_count, operandT **operands, size_t *operand_count, Register *registers, size_t register_count){

    size_t i;
    size_t j;
    size_t k;
    Operand *classified;

    for (i = 0; i < instruction_count; i++)
    {
        printf("Line %zu: ", contents[i].line_no);

        for (j = 0; j < instructions[i].len; j++)
            printf("%c", content[instructions[i].start + j]);

        printf("\n");
        if (!classify_operands(content, operands[i], operand_count[i], registers, register_count, &classified))
        {
            printf("Unable to classify operands\n");
            continue;
        }
        for (j = 0; j < operand_count[i]; j++)
        {
            printf("    Operand %zu: ", j + 1);
             //operandT tells us where the operand exists
             //in the original source buffer.
            for (k = 0; k < operands[i][j].len; k++)
                printf("%c", content[operands[i][j].start + k]);
            printf(" -> ");
            if (classified[j].type == OPERAND_REGISTER)
                printf("REGISTER");
            else if (classified[j].type == OPERAND_IMMEDIATE)
                printf("IMMEDIATE");
            else if (classified[j].type == OPERAND_MEMORY)
                printf("MEMORY");
            else if (classified[j].type == OPERAND_LABEL)
                printf("LABEL");
            else
                printf("UNKNOWN");

            printf("\n");
        }
        free(classified);
    }
}

