#ifndef OPCODE_H
#define OPCODE_H

#include <stddef.h>

typedef struct
{
    char *name;
} Opcode;

int load_opcodes(const char *filename, Opcode **opcodes, size_t *opcode_count);
int is_opcode(char *content, size_t start, size_t len, Opcode *opcodes, size_t opcode_count);
void free_opcodes(Opcode *opcodes, size_t opcode_count);

#endif

