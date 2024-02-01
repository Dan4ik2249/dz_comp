#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


#define MAXSIZE 65535

enum {
	OP_ADD,
	OP_AND,
	OP_NOT,
	OP_TRAP
};

enum {
	REG_N0 = 0,
	REG_N1,
	REG_N2,
	REG_N3,
	REG_N4,
	REG_N5,
	REG_N6,
	REG_N7,
	REG_PRGC,
	REG_FLAG,
	REG_COUNT
};

enum {
	FL_POS = 1 << 0,
	FL_ZERO = 1 << 1,
	FL_NEG = 1 << 2
};

enum {
	TRAP_GET=0x20,
	TRAP_GETC=0x21,
	TRAP_PUT=0x22,
	TRAP_PUTS=0x23,
	TRAP_GETUINT=0x24,
	TRAP_PUTUINT=0x25,
	TRAP_HALT=0x26
};

uint16_t memory[MAXSIZE] = {0};
uint16_t reg[REG_COUNT] = {0};

uint16_t val_extend(uint16_t sign){
	if ((sign >> 4) & 1){
		sign |= (0xFFFF << 5);
	}
	return sign;
}

void update_flags(uint16_t val){
	if (val == 0){
		reg[REG_FLAG] = FL_ZERO;
	}
	else if ((val >> 15) == 1){
		reg[REG_FLAG] = FL_NEG;
	}
	else {
		reg[REG_FLAG] = FL_POS;
	}
}

void add(uint16_t instr){
	uint16_t r0 = (instr >> 9) & 0x7;
	uint16_t r1 = (instr >> 6) & 0x7;
	uint16_t mode = (instr >> 5) & 0x1;

	if (mode){
		uint16_t val = val_extend(instr & 0x1F);
		reg[r0] = reg[r1] + val;
	}
	else {
		uint16_t r2 = instr & 0x7;
		reg[r0] = reg[r1] + reg[r2];
	}
	
	update_flags(reg[r0]);
}

void and(uint16_t instr){
        uint16_t r0 = (instr >> 9) & 0x7;
        uint16_t r1 = (instr >> 6) & 0x7;
        uint16_t mode = (instr >> 5) & 0x1;

        if (mode){
                uint16_t val = val_extend(instr & 0x1F);
                reg[r0] = reg[r1] & val;
        }
        else {
                uint16_t r2 = instr & 0x7;
                reg[r0] = reg[r1] & reg[r2];
        }

        update_flags(reg[r0]);
}

void not(uint16_t instr){
	uint16_t r0 = (instr >> 9) & 0x7;
        uint16_t r1 = (instr >> 6) & 0x7;

	reg[r0] = ~reg[r1];
	update_flags(reg[r0]);
}

int read_file(char *filename){
	FILE *fd;
	if ((fd = fopen(filename, "rb")) == NULL){
		perror("File not open");
		return __LINE__;
	}
	size_t size = fread(memory+0x3000, sizeof(uint16_t), MAXSIZE-0x3000, fd);
	/*while (size-- > 0){
		
	}*/
	fclose(fd);
	return 0;
}

int main (int argc, char *argv[]){
	if (argc < 2){
		perror("Not found instruction");
		return __LINE__;
	}
	read_file(argv[1]);
	reg[REG_PRGC] = 0x3000;

	int run = 1;
	while(run){
		uint16_t instr = memory[reg[REG_PRGC]++];
		uint16_t op = instr >> 12;

		switch(op){
			case OP_ADD:
				{
					add(instr);
				}
				break;
			case OP_AND:
				{
					and(instr);
				}
				break;
			case OP_TRAP:
				{
					switch(instr & 0xFF){
						case TRAP_GET:
							{
								reg[REG_N0] = (uint16_t)getchar();
								update_flags(reg[REG_N0]);
							}
							break;
						case TRAP_GETC:
							{
								reg[REG_N0] = (uint16_t)getchar();
								update_flags(reg[REG_N0]);
								fprintf(stdout, "%c", (char)reg[REG_N0]);
							}
							break;
						case TRAP_PUT:
							{
								fprintf(stdout, "%c", (char)reg[REG_N0]);
							}
							break;
						case TRAP_PUTS:
							{
								uint16_t *str = memory + reg[REG_N0];
								while(*str){
									fprintf(stdout, "%c", (char)*str);
									str++;
								}
							}
							break;
						case TRAP_GETUINT:
							{
								fscanf(stdin, "%hu", &reg[REG_N0]);
								update_flags(reg[REG_N0]);	
							}
							break;
						case TRAP_PUTUINT:
							{
								fprintf(stdout, "%hu \n", reg[REG_N0]);
							}
							break;
						case TRAP_HALT:
							{
								run = 0;
							}
							break;
					}
				}
				break;
			default:
				{
					perror("Not found instruct");
					break;	
				}
		
		}
	}
	return 0;
}


