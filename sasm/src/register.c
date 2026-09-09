#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "register.h"

int load_registers(const char *filename, Register **registers, size_t *register_count){
    FILE *fp;
    char name[32];
    int value;
    size_t count;
    size_t i;

    fp = fopen(filename, "r");

    if (fp == NULL){
        perror("fopen");
		return 0;
	}

     //First count the number of register definitions.
     //This allows us to allocate the complete register table once.
    count = 0;
    while (fscanf(fp, "%31s %d", name, &value) == 2)
        count++;
    
	rewind(fp);
    *registers = malloc(sizeof(Register) * count);

    if (*registers == NULL){
        fclose(fp);
        return 0;
    }
     //Read the register name and its IA-32 encoding.
    for (i = 0; i < count; i++){
        if (fscanf(fp, "%31s %d", name, &value) != 2){
            fclose(fp);
            return 0;
        }

        (*registers)[i].name = malloc(strlen(name) + 1);

        if ((*registers)[i].name == NULL){
            fclose(fp);
            return 0;
        }

        strcpy((*registers)[i].name, name);
        (*registers)[i].value = value;
    }

    *register_count = count;
    fclose(fp);
    return 1;
}

int is_register(char *content, size_t start, size_t len, Register *registers, size_t register_count)
{
    size_t i;
    for (i = 0; i < register_count; i++){
        if (strlen(registers[i].name) == len && strncmp(content + start, registers[i].name, len) == 0)
            return 1;
    }
    return 0;
}

/*
int register_value(char *content, size_t start, size_t len, Register *registers, size_t register_count, int *value)
{
    size_t i;
    for (i = 0; i < register_count; i++){
        if (strlen(registers[i].name) == len && strncmp(content + start, registers[i].name, len) == 0){
            *value = registers[i].value;
            return 1;
        }
    }
    return 0;
}
*/

void free_registers(Register *registers, size_t register_count)
{
    size_t i;
    for (i = 0; i < register_count; i++)
        free(registers[i].name);
    free(registers);
}

