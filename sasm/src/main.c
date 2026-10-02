#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "instruction.h"
#include "register.h"
#include "output.h"
#include "validate.h"
#include "opcode.h"

int main(int argc, char *argv[])
{
    char *content;
    size_t content_len;

    lineT *lines;
    size_t line_count;
	
	contentT *contents;
	size_t content_count;

	instructionT *instructions;
    size_t instruction_count;

	Opcode *opcodes;
	size_t opcode_count;
    
	operandT **operands;
    size_t *operand_count;
   
    Register *registers; 
	size_t register_count; 
	Operand *classified;

	size_t i;
    size_t j;

    if (argc != 2)
    {
        printf("Usage: %s <assembly-file>\n", argv[0]);
        return 1;
    }
	// Load the registers supported by our assembler.
	if(!load_registers("data/registers.txt", &registers, &register_count)){ 
		printf("Unable to load registers\n"); 
		return 1; 
	}

	if (!load_opcodes("data/opcode.txt", &opcodes, &opcode_count)){
    	printf("Unable to load opcodes\n");
    	free_registers(registers, register_count);
    	return 1;
	}

    if (!read_source(argv[1], &content, &content_len))
    {
        printf("Unable to read file\n");
        free_registers(registers, register_count);
        return 1;
    }

    if (!find_lines(content, content_len, &lines, &line_count))
    {
        printf("Unable to find lines\n");
        free(content);
        free_registers(registers, register_count);
        return 1;
    }

	if (!find_content(content, lines, line_count, &contents, &content_count))
    {
        printf("Unable to find content\n");
        free(lines);
        free(content);
        free_registers(registers, register_count);
        return 1;
    }
	/* first v1
    for (i = 0; i < line_count; i++)
    {
        printf("Line %zu: ", i + 1);
        for (j = 0; j < lines[i].len; j++)
        {
            printf("%c", content[lines[i].startidx + j]);
        }
        printf("\n");
    }
	
	v2
	for (i = 0; i < content_count; i++)
    {
        printf("Line %zu: startidx = %zu : ",
               contents[i].line_no,
               contents[i].startidx);

        for (j = contents[i].startidx;
             j < lines[i].startidx + lines[i].len;
             j++)
        {
            printf("%c", content[j]);
        }

        printf("\n");
    }
	*/
	if (!find_instructions(content, lines, contents, content_count, &instructions, &instruction_count))
    {
        printf("Unable to find instructions\n");

        free(contents);
        free(lines);
        free(content);
        free_registers(registers, register_count);
        return 1;
    }

	if (!find_operands(content, lines, contents, instructions, instruction_count, &operands, &operand_count)){
        printf("Unable to find operands\n");

        free(instructions);
        free(contents);
        free(lines);
        free(content);
        free_registers(registers, register_count);
		return 1;
    }

	if (!validate_instructions(content, contents, instructions, instruction_count, operands, operand_count, opcodes, opcode_count, registers, register_count)){
    	printf("Validation failed\n");
    	return 1;
	}
	//display_result(content, contents, instructions, instruction_count, operands, operand_count, registers, register_count);
	/*
    for (i = 0; i < instruction_count; i++){
        printf("Line %zu: ", contents[i].line_no);
        for (j = 0; j < instructions[i].len; j++){
            printf("%c",content[instructions[i].start + j]);
        }
        printf("\n");


        for (j = 0; j < operand_count[i]; j++){
            printf("    Operand %zu: ", j + 1);
            for (size_t k = 0; k < operands[i][j].len; k++){
                printf("%c", content[operands[i][j].start + k]);
            }
            printf("\n");
        }
    }*/
	for (i = 0; i < instruction_count; i++){
        free(operands[i]);
    }

    free_registers(registers, register_count);
    free_opcodes(opcodes, opcode_count);
    free(operands);
    free(operand_count);
	free(instructions);
    free(contents);
    free(lines);
    free(content);
    return 0;
}
