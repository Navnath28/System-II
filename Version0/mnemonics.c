#include<stdio.h>
#include<stdlib.h>
#define ERR 1
typedef struct{
	size_t startidx;	//newline start
	size_t len;			// line length
}lineT;

typedef struct{
	unsigned short nl;		//line no.
	size_t startidx;		//actual content start idx
}contentT;

typedef struct{
	size_t start;
	size_t len;
}instructionT;

int main(int argc, char **argv){
	if(argc != 2){
		fprintf(stderr,"Usage: %s <input file>\n",argv[0]);
		return ERR;
	}

	FILE *fp = fopen(argv[1],"rb");
	if(!fp){
		return ERR;
	}
	if(fseek(fp,0,SEEK_END)!=0){
        fclose(fp); return ERR;
    }
    size_t size = ftell(fp);
    if(size < 0){
        fclose(fp); return ERR;
    }
    if(fseek(fp,0,SEEK_SET) != 0){
        fclose(fp); return ERR;
    }
	
	// for actual instructions with whom we are
	// going to check .asm
	// we will keep instructions in sorted array
	/*
	FILE *fp2 = fopen(argv[2],"rb");
	if(!fp){
		return ERR;
	}
	*/
	char *instruction[] = {"add","cmp","dec","div","inc","mov","ret","sub"};
//======================================================
	unsigned char *buf = malloc(size+1);
	if(!buf){
		fclose(fp); return ERR;
	}
	size_t read = fread(buf,1,size,fp);
	buf[size] = '\0';
	if(read != size){
		if(ferror(fp)){
			free(buf);
			fclose(fp);
			return ERR;
		}
	}

//=======================================================
//		line obj
	size_t i=0,lncnt=0;
	while(i<size){
		if(buf[i] == '\n')
			lncnt++;
		i++;
	}
	printf("Size:%zu\n",lncnt);
	
	lineT *lines = malloc(lncnt*sizeof(lineT));
	if(!lines){
		free(buf);
		fclose(fp);
		return ERR;
	}
	i=0;
	size_t line_index = 0;
	size_t line_end = 0;
	size_t line_start = 0;
	while(i<size && line_index<lncnt){
		if(buf[i] == '\n'){
			lines[line_index].startidx = line_start;
			lines[line_index].len = i - line_start;
			line_start = i+1;
			line_index++;
		}
		i++;
	}
	for(i=0; i<lncnt; i++){
		printf("Line start: %zu | length: %zu\n",lines[i].startidx,lines[i].len);
	}

//========================================================
//		contentT
	contentT *content = malloc(lncnt*sizeof(contentT));
	i=0;
	size_t j,length=0,flag=0;
	while(i<lncnt){
		content[i].nl = i;
		j = lines[i].startidx;
		length = j + lines[i].len;
		flag = 0;
		while(j<length && flag==0){
			if(buf[j] != ' ' && buf[j] != '\t'){
				content[i].startidx = j;
				flag =1;
			}
			j++;
		}
		i++;
	}
	for(i=0; i<lncnt; i++){
		printf("Content lineno: %zu | startidx: %zu\n",content[i].nl,content[i].startidx);
	}

//========================================================
//		instructionT
	instructionT *inst = malloc(lncnt*sizeof(instructionT));
	i=0;
	length=0;
	j=0;
	while(i<lncnt){
		inst[i].start = content[i].startidx;
		j = content[i].startidx;
		length = j+lines[i].len;
		while(j<length && buf[j] != ' ' && buf[j] != '\n'){
			j++;
		}
		inst[i].len = j - inst[i].start;			// [ CHECK ]
		i++;
	}
	for(i=0; i<lncnt; i++){
		printf("Instruction start: %zu | length: %zu\n",inst[i].start,inst[i].len);
	}
	printf("All instruction:\n");
	for(i = 0; i < lncnt; i++){
    	printf("Instruction %zu: ", i + 1);
    	for(j = 0; j < inst[i].len; j++){
        	putchar(buf[inst[i].start + j]);
    	}
    	printf("\n");
	}


//=================================================================
//		VALIDATION
size_t instruction_count = sizeof(instruction) / sizeof(instruction[0]);
printf("\nInstruction Validation:\n");
for(i = 0; i < lncnt; i++){
    size_t low = 0;
    size_t high = instruction_count;
    int found = 0;
    while(low < high){
        size_t mid = low + (high - low) / 2;
        size_t k = 0;
        /* Find length of instruction[mid] */
        while(instruction[mid][k] != '\0')
            k++;
        /*
         * Compare mnemonic length first
         */
        if(inst[i].len < k){
            high = mid;
        }
        else if(inst[i].len > k){
            low = mid + 1;
        }
        else{
            /*
             * Same length.
             * Compare characters from buf.
             */
            k = 0;
            while(k < inst[i].len && buf[inst[i].start + k] == instruction[mid][k]){
                k++;
            }
            if(k == inst[i].len){
                found = 1;
                break;
            }
            /*
             * Find whether mnemonic is before
             * or after instruction[mid].
             */
            if(buf[inst[i].start + k] < instruction[mid][k])
                high = mid;
            else
                low = mid + 1;
        }
    }
		if(!found){
    	printf("Error line: %zu ", i + 1);
    	for(j = 0; j < inst[i].len; j++)
        	putchar(buf[inst[i].start + j]);
    	printf(" invalid instruction\n");
		}
	}
	return 0;
}
