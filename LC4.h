typedef struct {
	unsigned short pc;
	unsigned short instruction;
	unsigned short regFileWE;
	unsigned short regWriteReg; //if regFileWE
	unsigned short regWriteValue; //if regFileWE
	unsigned short nzpWE;
	unsigned short nzpWriteValue; //if nzpWE
	unsigned short dataWE;
	unsigned short dataAddr; //if ldr/str
	unsigned short dataValue; //if ldr/str
	unsigned short privilege;
	unsigned short nzp;

	short regs[8];
	unsigned short memory[65536];
} lc4;

lc4* init_lc4 ();

void parse_break (lc4 *l, unsigned short i);
void parse_arithmetic (lc4 *l, unsigned short i);
void parse_cmp (lc4 *l, unsigned short i);
void parse_jsr (lc4 *l, unsigned short i);
void parse_logic (lc4 *l, unsigned short i);
void parse_load (lc4 *l, unsigned short i);
void parse_store (lc4 *l, unsigned short i);
void parse_rti (lc4 *l, unsigned short i);
void parse_const (lc4 *l, unsigned short i);
void parse_modshift (lc4 *l, unsigned short i);
void parse_jmp (lc4 *l, unsigned short i);
void parse_hiconst (lc4 *l, unsigned short i);
void parse_trap (lc4 *l, unsigned short i);

char* run_instruction(lc4 *l);