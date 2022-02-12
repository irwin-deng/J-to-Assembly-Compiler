#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "LC4.h"

#define MEMORY_SIZE 65536

char buffer[46];
char binBuffer[17];

void reset_registers(lc4 *l) {
	for (int i=0; i<8; i++) {
		l->regs[i] = 0;
	}
}

void reset_memory(lc4 *l) {
	for (int i=0; i<MEMORY_SIZE; i++) {
		l->memory[i] = 0;
	}
}

void reset_buffer() {
	for(int i=0; i<46; i++) {
		buffer[i] = 0;
	}
}

lc4* init_lc4 () {
	lc4* l = (lc4*) malloc(sizeof(lc4));
	if(l == NULL) {
		exit (1);
	}

	l->pc = 0x8200;
	l->instruction = 0x0000;
	l->regFileWE = 1;
	l->regWriteValue = 0;
	l->nzpWE = 1;
	l->nzpWriteValue = 0;
	l->dataWE = 0;
	l->dataAddr = 0;
	l->dataValue = 0;
	l->nzp = 2;
	l->privilege = 1;
	reset_registers(l);
	reset_memory(l);

	return l;
}

short sext9 (lc4 *l, unsigned short instruction) {
	short out = instruction & 0x01FF;
	return out | ((out & 0x0100) ? 0xFE00 : 0);
}

short sext5 (lc4 *l, unsigned short instruction) {
	short out = instruction & 0x001F;
	return out | ((out & 0x0010) ? 0xFFE0 : 0);
}

unsigned short usext7 (lc4 *l, unsigned short instruction) {
	unsigned short out = instruction & 0x007F;
	return out;
}

unsigned short usext4 (lc4 *l, unsigned short instruction) {
	unsigned short out = instruction & 0x000F;
	return out;
}
unsigned short usext8 (lc4 *l, unsigned short instruction) {
	unsigned short out = instruction & 0x00FF;
	return out;
}

short sext7 (lc4 *l, unsigned short instruction) {
	short out = instruction & 0x007F;
	return out | ((out & 0x0040) ? 0xFF80 : 0);
}

short sext6 (lc4 *l, unsigned short instruction) {
	short out = instruction & 0x003F;
	return out | ((out & 0x0020) ? 0xFFC0 : 0);
}

short sext11 (lc4 *l, unsigned short instruction) {
	short out = instruction & 0x07FF;
	return out | ((out & 0x0400) ? 0xF800 : 0);
}

char *short_to_bin(short a) {
    for (int i = 15; i >= 0; i--) {
        if ((a & 0x0001) == 0x0001) {
        	binBuffer[i] = '1';
        } else {
        	binBuffer[i] = '0';
        }

        a >>= 1;
    }
    return binBuffer;
}

void parse_break (lc4 *l, unsigned short instruction) {
	short imm9 = sext9 (l, instruction);
		
	l->pc += 1;
	if(((instruction >> 9) & l->nzp) != 0) {
		l->pc += imm9;
	}

	l->regFileWE = 0;
	l->nzpWE = 0;
	l->dataWE = 0;

	return;
}

void parse_arithmetic (lc4 *l, unsigned short instruction) {
	unsigned short rd = (instruction >> 9) & 0x0007;
	unsigned short rs = (instruction >> 6) & 0x0007;
	unsigned short isAddImm = instruction & 0x0020;

	l->regFileWE = 1;
	l->regWriteReg = rd;
	l->nzpWE = 1;
	l->dataWE = 0;

	if(isAddImm) {
		short imm = sext5 (l, instruction);
		l->regs[rd] = l->regs[rs] + imm;
	} else {
		unsigned short rt = instruction & 0x0007;
		unsigned short subOpCode = (instruction >> 3) & 0x0003;

		if(subOpCode == 0x0000) {
			l->regs[rd] = l->regs[rs] + l->regs[rt];
		} else if (subOpCode == 0x0001) {
			l->regs[rd] = l->regs[rs] * l->regs[rt];
		} else if (subOpCode == 0x0002) {
			l->regs[rd] = l->regs[rs] - l->regs[rt];
		} else { //subOpCode == 0x0003
			l->regs[rd] = l->regs[rs] / l->regs[rt];
		}
	}
	l->regWriteValue = l->regs[rd];

	if(l->regs[rd] > 0) {
		l->nzpWriteValue = 1;
	} else if (l->regs[rd] < 0) {
		l->nzpWriteValue = 4;
	} else {
		l->nzpWriteValue = 2;
	}
	l->nzp = l->nzpWriteValue;
	l->pc++;
	return;
}

void parse_logic (lc4 *l, unsigned short instruction) {
	unsigned short rd = (instruction >> 9) & 0x0007;
	unsigned short rs = (instruction >> 6) & 0x0007;
	unsigned short isAndImm = instruction & 0x0020;

	l->regFileWE = 1;
	l->regWriteReg = rd;
	l->nzpWE = 1;
	l->dataWE = 0;

	if(isAndImm) {
		short imm = sext5 (l, instruction);
		l->regs[rd] = l->regs[rs] & imm;
	} else {
		unsigned short rt = instruction & 0x0007;
		unsigned short subOpCode = (instruction >> 3) & 0x0003;

		if(subOpCode == 0x0000) {
			l->regs[rd] = l->regs[rs] & l->regs[rt];
		} else if (subOpCode == 0x0001) {
			l->regs[rd] = ~l->regs[rs];
		} else if (subOpCode == 0x0002) {
			l->regs[rd] = l->regs[rs] | l->regs[rt];
		} else { //subOpCode == 0x0003
			l->regs[rd] = l->regs[rs] ^ l->regs[rt];
		}
	}
	l->regWriteValue = l->regs[rd];

	if(l->regs[rd] > 0) {
		l->nzpWriteValue = 1;
	} else if (l->regs[rd] < 0) {
		l->nzpWriteValue = 4;
	} else {
		l->nzpWriteValue = 2;
	}
	l->nzp = l->nzpWriteValue;
	l->pc++;
	return;
}

void parse_cmp (lc4 *l, unsigned short instruction) {
	unsigned short isImm = instruction & 0x0100;
	unsigned short rs = (instruction >> 9) & 0x0007;

	l->regFileWE = 0;
	l->nzpWE = 1;
	l->dataWE = 0;

	if(isImm) {
		if(instruction & 0x0080) { //cmpiu
			unsigned int imm = (unsigned int) usext7 (l, instruction);			if((unsigned) l->regs[rs] > imm) {
				l->nzpWriteValue = 1;
			} else if ((unsigned) l->regs[rs] < imm) {
				l->nzpWriteValue = 4;
			} else {
				l->nzpWriteValue = 2;
			}
		} else { //cmpi
			int imm = (int) sext7 (l, instruction);
			if(l->regs[rs] > imm) {
				l->nzpWriteValue = 1;
			} else if (l->regs[rs] < imm) {
				l->nzpWriteValue = 4;
			} else {
				l->nzpWriteValue = 2;
			}
		}
	} else {
		unsigned short rt = instruction & 0x0007;

		if(instruction & 0x0080) { //cmpu
			if((unsigned short) l->regs[rs] > (unsigned short) l->regs[rt]) {
				l->nzpWriteValue = 1;
			} else if ((unsigned short) l->regs[rs] < (unsigned short) l->regs[rt]) {
				l->nzpWriteValue = 4;
			} else {
				l->nzpWriteValue = 2;
			}
		} else { //cmp
			if(l->regs[rs] > l->regs[rt]) {
				l->nzpWriteValue = 1;
			} else if (l->regs[rs] < l->regs[rt]) {
				l->nzpWriteValue = 4;
			} else {
				l->nzpWriteValue = 2;
			}
		}
	}
	
	l->nzp = l->nzpWriteValue;
	l->pc++;
	return;
}

void parse_jsr (lc4 *l, unsigned short instruction) {
	l->regFileWE = 1;
	l->nzpWE = 1;
	l->dataWE = 0;	

	unsigned short isntRR = (instruction >> 11) & 1;
	short newR7 = l->pc + 1;

	if(isntRR) {
		short label = sext11 (l, instruction);
		l->pc = (l->pc & 0x8000) | (label << 4);
	} else {
		unsigned short rs = (instruction >> 6) & 0x0007;
		l->pc = l->regs[rs];
	}

	l->regs[7] = newR7;
	l->regWriteReg = 7;
	l->regWriteValue = l->regs[7];

	if(l->regs[7] > 0) {
		l->nzpWriteValue = 1;
	} else if (l->regs[7] < 0) {
		l->nzpWriteValue = 4;
	} else {
		l->nzpWriteValue = 2;
	}
	l->nzp = l->nzpWriteValue;
	return;
}

void parse_jmp (lc4 *l, unsigned short instruction) {
	l->regFileWE = 0;
	l->nzpWE = 0;
	l->dataWE = 0;	

	unsigned short isntR = (instruction >> 11) & 1;

	if(isntR) {
		l->pc += sext11 (l, instruction) + 1;	
	} else {
		unsigned short rs = (instruction >> 6) & 0x0007;
		l->pc = l->regs[rs];
	}
	
	return;
}

void parse_const (lc4 *l, unsigned short instruction) {
	l->regFileWE = 1;
	l->nzpWE = 1;
	l->dataWE = 0;	

	unsigned short rd = (instruction >> 9) & 0x0007;
	short imm = sext9 (l, instruction);

	l->regs[rd] = imm;
	l->regWriteReg  = rd;
	l->regWriteValue = l->regs[rd];	
	
	if(l->regs[rd] > 0) {
		l->nzpWriteValue = 1;
	} else if (l->regs[rd] < 0) {
		l->nzpWriteValue = 4;
	} else {
		l->nzpWriteValue = 2;
	}
	l->nzp = l->nzpWriteValue;
	l->pc++;
	return;
}
	
void parse_hiconst (lc4 *l, unsigned short instruction) {
	l->regFileWE = 1;
	l->nzpWE = 1;
	l->dataWE = 0;	

	unsigned short rd = (instruction >> 9) & 0x0007;
	unsigned short imm = usext8 (l, instruction);

	l->regs[rd] = (l->regs[rd] & 0xFF) | (imm << 8);
	l->regWriteReg  = rd;
	l->regWriteValue = l->regs[rd];	
	
	if(l->regs[rd] > 0) {
		l->nzpWriteValue = 1;
	} else if (l->regs[rd] < 0) {
		l->nzpWriteValue = 4;
	} else {
		l->nzpWriteValue = 2;
	}
	l->nzp = l->nzpWriteValue;
	l->pc++;
	return;
}
void parse_modshift (lc4 *l, unsigned short instruction) {
	l->regFileWE = 1;
	l->nzpWE = 1;
	l->dataWE = 0;

	unsigned short rd = (instruction >> 9) & 0x0007;
	unsigned short rs = (instruction >> 6) & 0x0007;
	unsigned short isMod = ((instruction >>4) & 0x0003) == 0x0003;

	if(isMod) {
		unsigned short rt = instruction & 0x0007;
		l->regs[rd] = l->regs[rs] % l->regs[rt];
	} else {
		unsigned short subOpCode = (instruction >> 4) & 0x0003;
		unsigned short imm = usext4 (l, instruction);

		if(subOpCode == 0x0000) {			//SLL
			l->regs[rd] = l->regs[rs] << imm;
		} else if (subOpCode == 0x0001) {	//SRA
			short temp = l->regs[rs];
			for (int i=0; i<imm; i++) {
				if (temp < 0) {
					temp >>= 1;
					temp |= 0x8000;
				} else {
					temp >>= 1;
				}
			}
			l->regs[rd] = temp;
		} else { //subOpCode == 0x0002		//SRL
			l->regs[rd] = ((unsigned short) l->regs[rs]) >> imm;
		}	
	}
	l->regWriteReg = rd;
	l->regWriteValue = l->regs[rd];
	
	if(l->regs[rd] > 0) {
		l->nzpWriteValue = 1;
	} else if (l->regs[rd] < 0) {
		l->nzpWriteValue = 4;
	} else {
		l->nzpWriteValue = 2;
	}
	l->nzp = l->nzpWriteValue;
	l->pc++;
	return;
}

void parse_load (lc4 *l, unsigned short instruction) {
	l->regFileWE = 1;
	l->nzpWE = 1;
	l->dataWE = 0;

	unsigned short rd = (instruction >> 9) & 0x0007;
	unsigned short rs = (instruction >> 6) & 0x0007;

	short imm = sext6 (l, instruction);
	l->dataAddr = l->regs[rs] + imm;
	l->regs[rd] = l->memory[l->dataAddr];

	l->regWriteValue = l->regs[rd];
	l->regWriteReg = rd;	
	
	if(l->regs[rd] > 0) {
		l->nzpWriteValue = 1;
	} else if (l->regs[rd] < 0) {
		l->nzpWriteValue = 4;
	} else {
		l->nzpWriteValue = 2;
	}
	l->nzp = l->nzpWriteValue;

	l->dataValue = l->regs[rd];
	l->pc++;
	return;
}

void parse_store (lc4 *l, unsigned short instruction) {
	l->regFileWE = 0;
	l->nzpWE = 0;
	l->dataWE = 1;

	unsigned short rt = (instruction >> 9) & 0x0007;
	unsigned short rs = (instruction >> 6) & 0x0007;

	short imm = sext6 (l, instruction);
	l->dataAddr = l->regs[rs] + imm;
	l->dataValue = l->regs[rt];

	l->memory[l->dataAddr] = l->dataValue;
	l->pc++;
	return;
}

void parse_trap (lc4 *l, unsigned short instruction) {
	l->regFileWE = 1;
	l->nzpWE = 1;
	l->dataWE = 0;
	
	unsigned short imm = usext8 (l, instruction);
	l->regs[7] = l->pc + 1;
	l->pc = (0x8000 | imm);
	l->privilege = 1;

	l->regWriteReg = 7;
	l->regWriteValue = l->regs[7];	

	if(l->regs[7] > 0) {
		l->nzpWriteValue = 1;
	} else if (l->regs[7] < 0) {
		l->nzpWriteValue = 4;
	} else {
		l->nzpWriteValue = 2;
	}
	l->nzp = l->nzpWriteValue;

	return;
}

void parse_rti (lc4 *l, unsigned short instruction) {
	l->regFileWE = 0;
	l->nzpWE = 0;
	l->dataWE = 0;
	
	l->pc = l->regs[7];
	l->privilege = 0;

	return;
}

char* run_instruction(lc4 *l) {
	unsigned short pc = l->pc;
	unsigned short instruction = l->instruction;
	unsigned short opCode = instruction >> 12;

	if (opCode == 0x0) { //break
		parse_break (l, instruction);
	} else if (opCode == 0x1) { //arithmetic
		parse_arithmetic (l, instruction);
	} else if (opCode == 0x2) { //CMP
		parse_cmp (l, instruction);
	} else if (opCode == 0x3) {
		return "INVALID INSTRUCTION";
	} else if (opCode == 0x4) { //JSR
		parse_jsr (l, instruction);
	} else if (opCode == 0x5) { //logic
		parse_logic (l, instruction);
	} else if (opCode == 0x6) { //LDR
		parse_load (l, instruction);
	} else if (opCode == 0x7) { //STR
		parse_store (l, instruction);
	} else if (opCode == 0x8) { //RTI
		parse_rti (l, instruction);
	} else if (opCode == 0x9) { //CONST
		parse_const (l, instruction);
	} else if (opCode == 0xA) { //modshift
		parse_modshift (l, instruction);
	} else if (opCode == 0xB) { //
		return "INVALID INSTRUCTION";
	} else if (opCode == 0xC) { //JMP
		parse_jmp (l, instruction);
	} else if (opCode == 0xD) { //HICONST
		parse_hiconst (l, instruction);
	} else if (opCode == 0xE) {
		return "INVALID INSTRUCTION";
	} else if (opCode == 0xF) { //TRAP
		parse_trap (l, instruction);
	}

	if (opCode != 0x6 && opCode != 0x7) { //if not LDR/STR
		l->dataAddr = 0;
		l->dataValue = 0;
	}

	if (l->regFileWE == 0) {
		l->regWriteReg = 0;
		l->regWriteValue = 0;
	}

	if (l->nzpWE == 0) {
		l->nzpWriteValue = 0;
	}

	reset_buffer();
	sprintf(buffer, "%04x %s %01x %01x %04x %01x %01x %01x %04x %04x",
		pc, short_to_bin(instruction), l->regFileWE, l->regWriteReg,
		l->regWriteValue, l->nzpWE, l->nzpWriteValue,
		l->dataWE, l->dataAddr, l->dataValue);

	return buffer;
}