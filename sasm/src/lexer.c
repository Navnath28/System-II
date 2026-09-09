#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

int read_source(const char *filename, char **content, size_t *content_len)
{
    FILE *fp;
    long size;

    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);
    *content = malloc(size + 1);
    if (*content == NULL)
    {
        fclose(fp);
        return 0;
    }

    *content_len = fread(*content, 1, size, fp);
    (*content)[*content_len] = '\0';
    fclose(fp);
    return 1;
}

int find_lines(char *content, size_t content_len, lineT **lines, size_t *line_count)
{
    size_t i;
    size_t start;
    size_t count;

    start = 0;
    count = 0;
    *lines = malloc(sizeof(lineT) * content_len);
    if (*lines == NULL)
    {
        return 0;
    }
    for (i = 0; i < content_len; i++)
    {
        if (content[i] == '\n')
        {
            (*lines)[count].startidx = start;
            (*lines)[count].len = i - start;

            count++;
            start = i + 1;
        }
    }
    if (start < content_len)
    {
        (*lines)[count].startidx = start;
        (*lines)[count].len = content_len - start;

        count++;
    }
    *line_count = count;
    return 1;
}

int find_content(char *content, lineT *lines, size_t line_count, contentT **contents, size_t *content_count)
{
    size_t i;
    size_t j;
    size_t start;
    *contents = malloc(sizeof(contentT) * line_count);
    if (*contents == NULL)
    {
        return 0;
    }
    for (i = 0; i < line_count; i++)
    {
        start = lines[i].startidx;
		j=0;
		while(j<lines[i].len && content[start+j] == ' ' || content[start+j] == '\t'){
			j++;
		}
        (*contents)[i].line_no = i + 1;
        (*contents)[i].startidx = start + j;
    }
    *content_count = line_count;
	return 1;
}

int find_instructions(char *content, lineT *lines, contentT *contents, size_t content_count, instructionT **instructions,
                      size_t *instruction_count)
{
    size_t i;
    size_t j;
    size_t start;
    size_t len;

    *instructions = malloc(sizeof(instructionT) * content_count);
    if (*instructions == NULL)
    {
        return 0;
    }
    for (i = 0; i < content_count; i++)
    {
        start = contents[i].startidx;
        len = 0;
        for (j = start;j < lines[i].startidx + lines[i].len;j++)
        {
            if (content[j] == ' ' || content[j] == '\t')
            {
                break;
            }
            len++;
        }
        (*instructions)[i].start = start;
        (*instructions)[i].len = len;
    }
    *instruction_count = content_count;
    return 1;
}

int find_operands(char *content, lineT *lines, contentT *contents, instructionT *instructions, size_t instruction_count, operandT ***operands, size_t **operand_count)
{
    size_t i;
    size_t j;
    size_t start;
    size_t end;
    size_t count;
    size_t line_end;
    *operands = malloc(sizeof(operandT *) * instruction_count);
    if (*operands == NULL){
        return 0;
    }
    *operand_count = malloc(sizeof(size_t) * instruction_count);
    if (*operand_count == NULL){
        free(*operands);
        return 0;
    }
    for (i = 0; i < instruction_count; i++){
        count = 0;
        line_end = lines[i].startidx + lines[i].len;
         //Start after the instruction.
        start = instructions[i].start +
                instructions[i].len;
         //Skip spaces and tabs after instruction.
        while (start < line_end && (content[start] == ' ' || content[start] == '\t'))
        {
            start++;
        }
         //Maximum of 3 operands for now.
        (*operands)[i] = malloc(sizeof(operandT) * 3);
        if ((*operands)[i] == NULL) return 0;
        while (start < line_end){
             //Find the end of the operand.
            end = start;
            while (end < line_end && content[end] != ',') end++;
            //Remove spaces/tabs at the end.
            j = end;
            while (j > start && (content[j - 1] == ' ' || content[j - 1] == '\t'))
            {
                j--;
            }
            (*operands)[i][count].start = start;
            (*operands)[i][count].len = j - start;
            count++;
             //Move past comma.
            if (end < line_end && content[end] == ',')
            {
                end++;
                while (end < line_end && (content[end] == ' ' || content[end] == '\t'))
                {
                    end++;
                }
            }
            start = end;
        }
        (*operand_count)[i] = count;
    }
    return 1;
}
