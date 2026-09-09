#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

#define ERR 1

typedef struct{
    size_t startidx;
    size_t len;
}lineT;

typedef struct{
    size_t nl;
    size_t startidx;
}contentT;

typedef struct{
    size_t start;
    size_t len;
}instructionT;

typedef struct{
    size_t start;
    size_t len;
}operandT;


int main(int argc, char **argv){

    if(argc != 3){
        fprintf(stderr,"Usage: %s <opcode file> <assembly file>\n",argv[0]);
        return ERR;
    }
//======================================================
//              OPCODE FILE
//======================================================

    FILE *fp = fopen(argv[1],"rb");

    if(!fp){
        return ERR;
    }

    if(fseek(fp,0,SEEK_END) != 0){
        fclose(fp);
        return ERR;
    }

    size_t opcode_size = ftell(fp);

    if(fseek(fp,0,SEEK_SET) != 0){
        fclose(fp);
        return ERR;
    }


//======================================================
//              Read opcode file
//======================================================

    unsigned char *opcode_buf = malloc(opcode_size + 1);

    if(!opcode_buf){
        fclose(fp);
        return ERR;
    }

    size_t read = fread(opcode_buf,1,opcode_size,fp);

    if(read != opcode_size){
        free(opcode_buf);
        fclose(fp);
        return ERR;
    }
    opcode_buf[opcode_size] = '\0';
    fclose(fp);
//------------------------------------------------------
//              Count opcode lines
//------------------------------------------------------

    size_t i = 0;
    size_t opcode_lncnt = 0;
    while(i < opcode_size){

        if(opcode_buf[i] == '\n')
            opcode_lncnt++;

        i++;
    }
    if(opcode_size > 0 && opcode_buf[opcode_size-1] != '\n')
        opcode_lncnt++;
//------------------------------------------------------
//              Extract instructions
//------------------------------------------------------

     //One entry for each opcode line.
     // duplicate entries are removed below.
    char **instruction = malloc(opcode_lncnt * sizeof(char *));
    if(!instruction){
        free(opcode_buf);
        return ERR;
    }
    size_t instruction_count = 0;
    size_t line_start = 0;
    size_t line_end = 0;

    i = 0;
    while(i < opcode_size){

        if(opcode_buf[i] == '\n'){
            line_end = i;
             // First field of opcode.txt ends at comma.
            size_t j = line_start;
            while(j < line_end && opcode_buf[j] != ',')j++;
            // ignore empty lines
			if(j > line_start){
                 // Check whether this instruction
                 // is already stored.
                size_t k = 0;
                int duplicate = 0;
                while(k < instruction_count){
                    size_t p = 0;
         /*chk*/   while(opcode_buf[line_start + p] != ',' && instruction[k][p] != '\0' && opcode_buf[line_start + p] == instruction[k][p]){
                        p++;
                    }
                    if(opcode_buf[line_start + p] == ',' &&
                       instruction[k][p] == '\0'){
                        duplicate = 1;
                        break;
                    }

                    k++;
                }

                if(!duplicate){
                     //Terminate mnemonic in opcode buffer.
                     //This allows instruction[] to point
                     //directly into opcode_buf.
                    opcode_buf[j] = '\0';
                    instruction[instruction_count] =
                        (char *)(opcode_buf + line_start);
                    instruction_count++;
                }
            }
            line_start = i + 1;
        }
        i++;
    }

    printf("Instructions from opcode.txt:\n");
    for(i = 0; i < instruction_count; i++)
        printf("%zu: %s\n",i + 1,instruction[i]);
//======================================================
// ASSEMBLY FILE
//======================================================

    fp = fopen(argv[2],"rb");
    if(!fp){
        free(instruction);
        free(opcode_buf);
        return ERR;
    }
    if(fseek(fp,0,SEEK_END) != 0){
        fclose(fp);
        free(instruction);
        free(opcode_buf);
        return ERR;
    }
    size_t size = ftell(fp);
    if(fseek(fp,0,SEEK_SET) != 0){
        fclose(fp);
        free(instruction);
        free(opcode_buf);
        return ERR;
    }
// Read assembly file
    unsigned char *buf = malloc(size + 1);

    if(!buf){
        fclose(fp);
        free(instruction);
        free(opcode_buf);
        return ERR;
    }
    read = fread(buf,1,size,fp);
    if(read != size){
        free(buf);
        fclose(fp);
        free(instruction);
        free(opcode_buf);
        return ERR;
    }
    buf[size] = '\0';
    fclose(fp);
//======================================================
// lineT
//======================================================
    size_t lncnt = 0;
    i = 0;
    while(i < size){
        if(buf[i] == '\n')
            lncnt++;
        i++;
    }

    if(size > 0 && buf[size-1] != '\n')lncnt++;
    printf("\nAssembly lines: %zu\n",lncnt);
    lineT *lines = malloc(lncnt * sizeof(lineT));
    if(!lines){
        free(buf);
        free(instruction);
        free(opcode_buf);
        return ERR;
    }
	i = 0;
    size_t line_index = 0;
    line_start = 0;
    while(i < size && line_index < lncnt){

        if(buf[i] == '\n'){
            lines[line_index].startidx = line_start;
            lines[line_index].len = i - line_start;

            line_start = i + 1;
            line_index++;
        }
        i++;
    }

     //Last line if file doesn't end with newline.
    if(line_start < size && line_index < lncnt){

        lines[line_index].startidx = line_start;
        lines[line_index].len = size - line_start;
        line_index++;
    }
//======================================================
// contentT
//======================================================

    contentT *content = malloc(lncnt * sizeof(contentT));
    if(!content){
        free(lines);
        free(buf);
        free(instruction);
        free(opcode_buf);
        return ERR;
    }
    i = 0;
    size_t j;
    size_t length;
    while(i < lncnt){
        content[i].nl = i;
        j = lines[i].startidx;
        length = lines[i].startidx + lines[i].len;
         //Skip spaces and tabs.
        while(j < length && (buf[j] == ' ' || buf[j] == '\t')){
            j++;
        }

        content[i].startidx = j;
        i++;
    }
//======================================================
// instructionT
//======================================================

    instructionT *inst = malloc(lncnt * sizeof(instructionT));
    if(!inst){
        free(content);
        free(lines);
        free(buf);
        free(instruction);
        free(opcode_buf);
        return ERR;
    }
    i = 0;
    while(i < lncnt){
        inst[i].start = content[i].startidx;
        j = content[i].startidx;
        length = lines[i].startidx + lines[i].len;
         //Find end of mnemonic.
        while(j < length && buf[j] != ' ' && buf[j] != '\t' && buf[j] != '\n' && buf[j] != ';'){
            j++;
        }
        inst[i].len = j - inst[i].start;
        i++;
    }
//======================================================
// MNEMONIC VALIDATION bnary search
//======================================================

    printf("\nInstruction Validation:\n");
    i = 0;
    while(i < lncnt){
        size_t low = 0;
        size_t high = instruction_count;
        int found = 0;
		while(low < high){
    		size_t mid = low + (high - low) / 2;
    		size_t k = 0;
    		//Compare characters */
    		while(k < inst[i].len && instruction[mid][k] != '\0' && buf[inst[i].start + k] == instruction[mid][k]){
        		k++;
    		}
    		//Complete match */
    		if(k == inst[i].len && instruction[mid][k] == '\0'){
        		found = 1;
        		break;
    		}
    		//Compare alphabetically */
    		if(k < inst[i].len && instruction[mid][k] != '\0'){
        		if(buf[inst[i].start + k] < instruction[mid][k])
            	high = mid;
        		else
            		low = mid + 1;
    		}
    		else if(k == inst[i].len){
        		high = mid;
    		}
    		else{
        		low = mid + 1;
    		}
		}
		if(!found){
        	printf("Error line: %zu ", i + 1);
        	for(j = 0; j < inst[i].len; j++)
            	putchar(buf[inst[i].start + j]);
        		printf(" invalid instruction\n");
    	}
        i++;
    }

//======================================================
//              operandT
//======================================================
     //Maximum of two operands per instruction
    operandT *operands = malloc(lncnt * 2 * sizeof(operandT));
    if(!operands){
        free(inst);
        free(content);
        free(lines);
        free(buf);
        free(instruction);
        free(opcode_buf);
        return ERR;
    }

   	size_t *operand_count = malloc(lncnt * sizeof(size_t));
    if(!operand_count){
        free(operands);
        free(inst);
        free(content);
        free(lines);
        free(buf);
        free(instruction);
        free(opcode_buf);
        return ERR;
    }
//======================================================
//              Extract operands
//======================================================

    i = 0;
    while(i < lncnt){
        operand_count[i] = 0;
         //Start after mnemonic.
        j = inst[i].start + inst[i].len;
        length = lines[i].startidx + lines[i].len;
         //Skip whitespace.
        while(j < length && (buf[j] == ' ' || buf[j] == '\t')){
            j++;
        }
         //Remove comment from operand scanning.
        size_t comment = j;
        while(comment < length){
            if(buf[comment] == ';'){
                length = comment;
                break;
            }
            comment++;
        }
         //No operand.
        if(j >= length){
            i++;
            continue;
        }
         //First operand.
        operands[i * 2].start = j;
        while(j < length && buf[j] != ','){
            j++;
        }
         //Remove trailing whitespace.
        size_t operand_end = j;
        while(operand_end > operands[i * 2].start && (buf[operand_end - 1] == ' ' || buf[operand_end - 1] == '\t')){
            operand_end--;
        }
        operands[i * 2].len = operand_end - operands[i * 2].start;
        if(operands[i * 2].len > 0)
            operand_count[i]++;
         //Second operand.
        if(j < length && buf[j] == ','){
            j++;
            while(j < length && (buf[j] == ' ' || buf[j] == '\t')){
                j++;
            }
            if(j < length){
                operands[i * 2 + 1].start = j;
                while(j < length && buf[j] != ',' ){
                    j++;
                }
                operand_end = j;
                while(operand_end > operands[i * 2 + 1].start &&
                      (buf[operand_end - 1] == ' ' || buf[operand_end - 1] == '\t')){
                    operand_end--;
                }
                operands[i * 2 + 1].len = operand_end - operands[i * 2 + 1].start;
                if(operands[i * 2 + 1].len > 0)
                    operand_count[i]++;
            }
        }
        i++;
    }
//======================================================
// Operand identification
//======================================================
    printf("\nOperand Identification:\n");
    i = 0;
    while(i < lncnt){
        if(inst[i].len == 0){
            i++;
            continue;
        }
        printf("Line %zu: ",i + 1);
         //Print mnemonic.
        for(j = 0; j < inst[i].len; j++)
            putchar(buf[inst[i].start + j]);
        printf("\n");
        size_t op = 0;
        while(op < operand_count[i]){
            operandT current = operands[i * 2 + op];
             //Print operand itself.
            printf("  Operand %zu: ",op + 1);
            for(j = 0; j < current.len; j++)
                putchar(buf[current.start + j]);
            printf(" -> ");
             //register
            char *registers[] = {"eax","ebx","ecx","edx","esi","edi","esp","ebp"};
            size_t register_count = sizeof(registers)/sizeof(registers[0]);
            int is_register = 0;
            size_t r = 0;
            while(r < register_count){
                size_t k = 0;
                while(k < current.len && registers[r][k] != '\0' && tolower(buf[current.start + k]) == registers[r][k]){
                    k++;
                }
                if(k == current.len &&
                   registers[r][k] == '\0'){
                    is_register = 1;
                    break;
                }
                r++;
            }
             //memory
            int is_memory = 0;
            if(current.len >= 2 && buf[current.start] == '[' && buf[current.start + current.len - 1] == ']'){
                is_memory = 1;
            }
             //constant
            int is_constant = 1;
            size_t k = 0;
             /*
			 * Decimal / hexadecimal constant.
             *
             * Accepted examples:
             * 10
             * 123
             * 0x10
             * 10h
             */
            if(current.len == 0){
                is_constant = 0;
            }
            else{
                size_t p = current.start;
                if(buf[p] == '-' || buf[p] == '+')
                    p++;
                if(p >= current.start + current.len){
                    is_constant = 0;
                }
                else{
                    while(p < current.start + current.len){
                        if(!isxdigit(buf[p])){
                             //Allow x/X only in 0x form
                            if(buf[p] != 'x' &&
                               buf[p] != 'X'){
                                is_constant = 0;
                                break;
                            }
                        }
                        p++;
                    }
                }
            }
             //print

            if(is_register)
                printf("Register\n");
            else if(is_memory)
                printf("Memory\n");
            else if(is_constant)
                printf("Constant\n");
            else
                printf("Symbol\n");
            op++;
        }
        i++;
    }
//======================================================
// CLEANUP
//======================================================
    free(operand_count);
    free(operands);
    free(inst);
    free(content);
    free(lines);
    free(buf);
    free(instruction);
    free(opcode_buf);

    return 0;
}
