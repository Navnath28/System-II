#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

typedef struct
{
    size_t startidx;
    size_t len;
} lineT;

typedef struct
{
    size_t line_no;
    size_t startidx;
} contentT;

typedef struct{
	size_t start;
	size_t len;
}instructionT;

typedef struct
{
    size_t start;
    size_t len;
} operandT;

int read_source(const char *filename, char **content, size_t *content_len);

int find_lines(char *content, size_t content_len, lineT **lines, size_t *line_count);

int find_content(char *content, lineT *lines, size_t line_count, contentT **contents, size_t *content_count);

int find_instructions(char *content, lineT *lines, contentT *contents, size_t content_count, instructionT **instructions, size_t *instruction_count);

int find_operands(char *content, lineT *lines, contentT *contents, instructionT *instructions, size_t instruction_count, operandT ***operands, size_t **operand_count);

#endif
