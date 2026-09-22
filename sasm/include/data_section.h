#ifndef DATA_H
#define DATA_H

#include <stddef.h>

#include "lexer.h"

typedef enum
{
    DATA_BYTE,
    DATA_WORD,
    DATA_DOUBLE
} DataType;

typedef struct
{
    size_t line_no;

    size_t label_start;
    size_t label_len;

    DataType type;

    size_t value_start;
    size_t value_len;

} dataT;

typedef struct
{
    size_t offset;
    char *opcode;
} dataOutputT;

int find_data(char *content, lineT *lines, contentT *contents, size_t content_count,
              dataT **data, size_t *data_count);

int encode_data(char *content, dataT *data, size_t data_count,
                dataOutputT **output, size_t *output_count);

void free_data(dataT *data);

void free_data_output(dataOutputT *output, size_t output_count);

#endif
