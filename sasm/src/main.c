#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "instruction.h"
#include "register.h"
#include "data_section.h"

/* * Temporary function for displaying the result of * lexical analysis and operand classification. * * This function will eventually be moved to output.c. */ 
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
int main(int argc, char *argv[])
{
    char *content;
    size_t content_len;

    lineT *lines;
    size_t line_count;
	
    contentT *contents;
    size_t content_count;

    	// data section
    dataT *data;
    size_t data_count;

    dataOutputT *data_output;
    size_t data_output_count;

    instructionT *instructions;
    size_t instruction_count;
    
    operandT **operands;
    size_t *operand_count;
   
    Register *registers; 
	size_t register_count; 
	Operand *classified;
 
    size_t i;
    size_t j;
    
    data = NULL;
    data_count = 0;

    data_output = NULL;
    data_output_count = 0;

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
    
    if (!find_data(content, lines, contents, content_count, &data, &data_count))
    {
	    printf("Unable to find data\n");

	    free(contents);
	    free(lines);
	    free(content);
	    free_registers(registers, register_count);

	    return 1;
    }
    
    if (!encode_data(content, data, data_count,
                &data_output, &data_output_count))
    {
	    printf("Unable to encode data\n");

	    free_data(data);
	    free(contents);
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
	*/
	
	
	for (i = 0; i < data_output_count; i++)
	{
	    printf("%08zX %s\n",
		   data_output[i].offset,
		   data_output[i].opcode);
	}

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

	display_result(content, contents, instructions, instruction_count, operands, operand_count, registers, register_count);
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

    free(operands);
    free(operand_count);
	free(instructions);
    free(contents);
    free(lines);
    free(content);
    return 0;
}
