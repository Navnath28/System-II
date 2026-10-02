#include <stdio.h>
#include <stdlib.h>
#include "validate.h"
#include "error.h"

int validate_instructions(char *content, contentT *contents, instructionT *instructions, size_t instruction_count, operandT **operands, size_t *operand_count, Opcode *opcodes, size_t opcode_count, Register *registers, size_t register_count)
{
	size_t i;
    size_t j;
    Operand *classified;

    for (i = 0; i < instruction_count; i++){
		if (!is_opcode(content, instructions[i].start, instructions[i].len, opcodes, opcode_count)){
            invalid_instruction(content, &instructions[i], contents[i].line_no);
            continue;
        }

        if (!classify_operands(content, operands[i], operand_count[i], registers, register_count, &classified))
            return 0;

        for (j = 0; j < operand_count[i]; j++){
            if (classified[j].type == OPERAND_UNKNOWN)
                invalid_operand(content, &operands[i][j], contents[i].line_no);
        }

        free(classified);
    }

    return 1;
}
