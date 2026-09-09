#include <stdlib.h>
#include <ctype.h>

#include "instruction.h"

 //Check whether the operand contains a decimal integer.
int is_number(char *content, size_t start, size_t len)
{
    size_t i;

    if (len == 0)
        return 0;

    i = 0;
    if (content[start] == '-' || content[start] == '+')
        i++;

    if (i == len)
        return 0;

    for (; i < len; i++){
        if (!isdigit((unsigned char)content[start + i]))
            return 0;
    }
    return 1;
}

 //Convert the immediate operand from source text into an integer.
int number_value(char *content, size_t start, size_t len, int *value){
    size_t i;
    int sign;
    int number;

    i = 0;
    sign = 1;
    number = 0;
    if (content[start] == '-'){
        sign = -1;
        i++;
    }
    else if (content[start] == '+'){
        i++;
    }
    for (; i < len; i++)
        number = number * 10 + (content[start + i] - '0');
    *value = number * sign;
    return 1;
}

 //For now, recognize memory operands using square brackets.
 //Examples: [eax] [ebx]
int is_memory(char *content, size_t start, size_t len)
{
    if (len < 2)
        return 0;
    if (content[start] == '[' && content[start + len - 1] == ']')
        return 1;
    return 0;
}

 //Convert lexical operands into semantic operands.
 //operandT tells us where the operand is.
 //Operand tells us what the operand means.
int classify_operands(char *content, operandT *operands, size_t operand_count, Register *registers, size_t register_count, Operand **classified)
{
    size_t i;
    int value;

    *classified = malloc(sizeof(Operand) * operand_count);

    if (*classified == NULL && operand_count != 0)
        return 0;

    for (i = 0; i < operand_count; i++)
    {
        (*classified)[i].type = OPERAND_UNKNOWN;
        (*classified)[i].value = 0;

        /*
         * Register:
         *
         *     eax -> REGISTER, 0
         *     ecx -> REGISTER, 1
         */
        if (is_register(content, operands[i].start, operands[i].len, registers, register_count)){
            // not now
			//register_value(content, operands[i].start, operands[i].len, registers, register_count, &value);
            //(*classified)[i].value = value;

            (*classified)[i].type = OPERAND_REGISTER;
        }
         //Immediate: 10 = IMMEDIATE, 10 -5 = IMMEDIATE, -5
        else if (is_number(content, operands[i].start, operands[i].len)){
            // not now
			//number_value(content, operands[i].start, operands[i].len, &value);
            //(*classified)[i].value = value;
            
			(*classified)[i].type = OPERAND_IMMEDIATE;
        }

         //Memory:
         //[eax] -> MEMORY
        // Memory operands will need a richer representation later
        // when we implement ModR/M and SIB encoding.
        else if (is_memory(content, operands[i].start, operands[i].len)){
            (*classified)[i].type = OPERAND_MEMORY;
            //(*classified)[i].value = 0;
        }

         //Anything that is not recognized yet is treated as a label.
        else{
            (*classified)[i].type = OPERAND_UNKNOWN;
            //(*classified)[i].value = 0;
        }
    }
    return 1;
}

