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
// OPCODE FILE

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


// Read opcode file

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

// Count opcode lines
    size_t i = 0;
    size_t opcode_lncnt = 0;
    while(i < opcode_size){

        if(opcode_buf[i] == '\n')
            opcode_lncnt++;

        i++;
    }
    if(opcode_size > 0 && opcode_buf[opcode_size-1] != '\n')
        opcode_lncnt++;

//  Extract instructions

		// [ NAVNATH ]

//--------------------------------------------------------------------------------
// ASSEMBLY FILE

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
//--------------------------------------------------------------------------------
// lineT
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
//--------------------------------------------------------------------------------
// contentT

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
//--------------------------------------------------------------------------------
// instructionT

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
//--------------------------------------------------------------------------------
// MNEMONIC VALIDATION bnary search

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

//--------------------------------------------------------------------------------
//              operandT
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
//--------------------------------------------------------------------------------
// Extract operands

		//  [ MAYURI  ]

//--------------------------------------------------------------------------------
// Operand identification

		// [ SACHIN  ]

//--------------------------------------------------------------------------------
// CLEANUP
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
