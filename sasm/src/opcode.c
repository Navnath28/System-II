#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opcode.h"

int load_opcodes(const char *filename, Opcode **opcodes, size_t *opcode_count)
{
    FILE *fp;
    size_t opcode_size;
    unsigned char *opcode_buf;
    size_t read;
    size_t i;
    size_t line_count;
    size_t line_start;
    size_t line_end;
    size_t instruction_count;
    Opcode *instruction;

    fp = fopen(filename, "rb");

    if (!fp)
        return 0;

    if (fseek(fp, 0, SEEK_END) != 0)
    {
        fclose(fp);
        return 0;
    }

    opcode_size = ftell(fp);

    if (fseek(fp, 0, SEEK_SET) != 0)
    {
        fclose(fp);
        return 0;
    }

    opcode_buf = malloc(opcode_size + 1);

    if (!opcode_buf)
    {
        fclose(fp);
        return 0;
    }

    read = fread(opcode_buf, 1, opcode_size, fp);

    if (read != opcode_size)
    {
        free(opcode_buf);
        fclose(fp);
        return 0;
    }

    opcode_buf[opcode_size] = '\0';
    fclose(fp);

    line_count = 0;
    i = 0;

    while (i < opcode_size)
    {
        if (opcode_buf[i] == '\n')
            line_count++;

        i++;
    }

    if (opcode_size > 0 && opcode_buf[opcode_size - 1] != '\n')
        line_count++;

    instruction = malloc(line_count * sizeof(Opcode));

    if (!instruction)
    {
        free(opcode_buf);
        return 0;
    }

    instruction_count = 0;
    line_start = 0;
    i = 0;

    while (i < opcode_size)
    {
        if (opcode_buf[i] == '\n')
        {
            line_end = i;

            size_t j = line_start;

            while (j < line_end && opcode_buf[j] != ',')
                j++;

            if (j > line_start)
            {
                size_t k;
                int duplicate;

                duplicate = 0;
                k = 0;

                while (k < instruction_count)
                {
                    if (strlen(instruction[k].name) == j - line_start && strncmp(instruction[k].name, (char *)(opcode_buf + line_start), j - line_start) == 0)
                    {
                        duplicate = 1;
                        break;
                    }

                    k++;
                }

                if (!duplicate)
                {
                    instruction[instruction_count].name = malloc(j - line_start + 1);

                    if (!instruction[instruction_count].name)
                    {
                        while (instruction_count > 0)
                        {
                            instruction_count--;
                            free(instruction[instruction_count].name);
                        }

                        free(instruction);
                        free(opcode_buf);
                        return 0;
                    }

                    memcpy(instruction[instruction_count].name, opcode_buf + line_start, j - line_start);
                    instruction[instruction_count].name[j - line_start] = '\0';

                    instruction_count++;
                }
            }

            line_start = i + 1;
        }

        i++;
    }

    *opcodes = instruction;
    *opcode_count = instruction_count;

    free(opcode_buf);

    return 1;
}

int is_opcode(char *content, size_t start, size_t len, Opcode *opcodes, size_t opcode_count) { 
	size_t low; 
	size_t high; 
	low = 0; 
	high = opcode_count; 
	while (low < high) { 
		size_t mid; 
		size_t i; 
		mid = low + (high - low) / 2; 
		i = 0; 
		while (i < len && opcodes[mid].name[i] != '\0' && content[start + i] == opcodes[mid].name[i]) i++; 
		if (i == len && opcodes[mid].name[i] == '\0') 
			return 1; 
		if (i < len && opcodes[mid].name[i] != '\0') { 
			if (content[start + i] < opcodes[mid].name[i]) 
				high = mid; 
			else low = mid + 1; 
			} 
		else if (i == len) { 
			high = mid; 
		} else { 
			low = mid + 1; 
		} 
	} 
	return 0; 
}

void free_opcodes(Opcode *opcodes, size_t opcode_count)
{
    size_t i;

    for (i = 0; i < opcode_count; i++)
        free(opcodes[i].name);

    free(opcodes);
}

