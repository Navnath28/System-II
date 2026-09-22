#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "data_section.h"

static int data_type(char *content, size_t start, size_t len, DataType *type)
{
    if (len == 2 && content[start] == 'd' && content[start + 1] == 'b')
    {
        *type = DATA_BYTE;
        return 1;
    }

    if (len == 2 && content[start] == 'd' && content[start + 1] == 'w')
    {
        *type = DATA_WORD;
        return 1;
    }

    if (len == 2 && content[start] == 'd' && content[start + 1] == 'd')
    {
        *type = DATA_DOUBLE;
        return 1;
    }

    return 0;
}

static int data_size(DataType type)
{
    if (type == DATA_BYTE)
        return 1;

    if (type == DATA_WORD)
        return 2;

    if (type == DATA_DOUBLE)
        return 4;

    return 0;
}

static size_t skip_spaces(char *content, size_t start, size_t end)
{
    while (start < end &&
           (content[start] == ' ' || content[start] == '\t'))
        start++;

    return start;
}

static size_t find_token_end(char *content, size_t start, size_t end)
{
    while (start < end &&
           content[start] != ' ' &&
           content[start] != '\t')
        start++;

    return start;
}

int find_data(char *content, lineT *lines, contentT *contents,
              size_t content_count, dataT **data, size_t *data_count)
{
    size_t i;
    size_t start;
    size_t end;
    size_t token_start;
    size_t token_end;
    size_t count;
    DataType type;
    int in_data;

    *data = malloc(sizeof(dataT) * content_count);

    if (*data == NULL && content_count != 0)
        return 0;

    count = 0;
    in_data = 0;

    for (i = 0; i < content_count; i++)
    {
        start = contents[i].startidx;
        end = lines[i].startidx + lines[i].len;

        start = skip_spaces(content, start, end);

        if (start == end)
            continue;

        /* Check for a section declaration. */
        token_start = start;
        token_end = find_token_end(content, token_start, end);

        if (token_end - token_start == 7 &&
            strncmp(content + token_start, "section", 7) == 0)
        {
            start = skip_spaces(content, token_end, end);

            token_start = start;
            token_end = find_token_end(content, token_start, end);

            if (token_end - token_start == 5 &&
                strncmp(content + token_start, ".data", 5) == 0)
                in_data = 1;
            else
                in_data = 0;

            continue;
        }

        if (!in_data)
            continue;

        /*
         * Data declaration:
         *
         * label directive value
         */

        token_start = start;
        token_end = find_token_end(content, token_start, end);

        if (token_end == start)
            continue;

        (*data)[count].line_no = contents[i].line_no;

        (*data)[count].label_start = token_start;
        (*data)[count].label_len = token_end - token_start;

        start = skip_spaces(content, token_end, end);

        token_start = start;
        token_end = find_token_end(content, token_start, end);

        if (!data_type(content, token_start,
                       token_end - token_start, &type))
            continue;

        (*data)[count].type = type;

        start = skip_spaces(content, token_end, end);

        if (start == end)
        {
            free(*data);
            *data = NULL;
            return 0;
        }

        (*data)[count].value_start = start;
        (*data)[count].value_len = end - start;

        count++;
    }

    *data_count = count;

    return 1;
}

static int is_number(char *content, size_t start, size_t len)
{
    size_t i;

    if (len == 0)
        return 0;

    i = 0;

    if (content[start] == '-' || content[start] == '+')
        i++;

    if (i == len)
        return 0;

    for (; i < len; i++)
    {
        if (!isdigit((unsigned char)content[start + i]))
            return 0;
    }

    return 1;
}

static int number_value(char *content, size_t start, size_t len, int *value)
{
    size_t i;
    int sign;
    int number;

    i = 0;
    sign = 1;
    number = 0;

    if (content[start] == '-')
    {
        sign = -1;
        i++;
    }
    else if (content[start] == '+')
    {
        i++;
    }

    for (; i < len; i++)
        number = number * 10 + (content[start + i] - '0');

    *value = number * sign;

    return 1;
}

static char *number_opcode(int value, int size)
{
    char *opcode;
    unsigned int number;
    int i;

    opcode = malloc((size * 2) + 1);

    if (opcode == NULL)
        return NULL;

    number = (unsigned int)value;

    for (i = 0; i < size; i++)
        sprintf(opcode + (i * 2),
                "%02X",
                (number >> (i * 8)) & 0xFF);

    opcode[size * 2] = '\0';

    return opcode;
}

static char *string_opcode(char *content, size_t start, size_t len)
{
    size_t i;
    size_t string_len;
    char *opcode;

    if (len < 2)
        return NULL;

    if (content[start] != '"' ||
        content[start + len - 1] != '"')
        return NULL;

    string_len = len - 2;

    opcode = malloc((string_len * 2) + 1);

    if (opcode == NULL)
        return NULL;

    for (i = 0; i < string_len; i++)
        sprintf(opcode + (i * 2),
                "%02X",
                (unsigned char)content[start + 1 + i]);

    opcode[string_len * 2] = '\0';

    return opcode;
}

int encode_data(char *content, dataT *data, size_t data_count,
                dataOutputT **output, size_t *output_count)
{
    size_t i;
    size_t offset;
    int value;
    int size;
    char *opcode;

    *output = malloc(sizeof(dataOutputT) * data_count);

    if (*output == NULL && data_count != 0)
        return 0;

    offset = 0;

    for (i = 0; i < data_count; i++)
    {
        size = data_size(data[i].type);
        opcode = NULL;

        if (data[i].value_len >= 2 &&
            content[data[i].value_start] == '"' &&
            content[data[i].value_start + data[i].value_len - 1] == '"')
        {
            if (size != 1)
            {
                free_data_output(*output, i);
                return 0;
            }

            opcode = string_opcode(content,
                                    data[i].value_start,
                                    data[i].value_len);
        }
        else if (is_number(content,
                           data[i].value_start,
                           data[i].value_len))
        {
            if (!number_value(content,
                              data[i].value_start,
                              data[i].value_len, &value))
            {
                free_data_output(*output, i);
                return 0;
            }

            opcode = number_opcode(value, size);
        }

        if (opcode == NULL)
        {
            free_data_output(*output, i);
            return 0;
        }

        (*output)[i].offset = offset;
        (*output)[i].opcode = opcode;

        offset += strlen(opcode) / 2;
    }

    *output_count = data_count;

    return 1;
}

void free_data(dataT *data)
{
    free(data);
}

void free_data_output(dataOutputT *output, size_t output_count)
{
    size_t i;

    for (i = 0; i < output_count; i++)
        free(output[i].opcode);

    free(output);
}
