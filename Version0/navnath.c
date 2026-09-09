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

			// 	NAVNATH

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
