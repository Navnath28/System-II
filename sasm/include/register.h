#ifndef REGISTER_H
#define REGISTER_H

#include <stddef.h>


typedef struct
{
    char *name;
	int value;		//further can be reduced as it only keeps 3 bits 000 - 111
} Register;


int load_registers(const char *filename, Register **registers, size_t *register_count);

int is_register(char *content, size_t start, size_t len, Register *registers, size_t register_count);

int register_value(char *content, size_t start, size_t len, Register *registers, size_t register_count, int *value);

void free_registers(Register *registers, size_t register_count);

#endif
