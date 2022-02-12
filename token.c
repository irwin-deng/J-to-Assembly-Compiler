#define MAX_TOKEN_LENGTH 250

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"

int defun = 0; //1 if last token was DEFUN; 0 otherwise
int num = 0; //number of comparisions + ifs
int last_ifs[1000];
int last_if = 0;
int max_if = 0;
int has_else = 0;

int valid_function_name (char * s) {
	int len = strlen(s);
	if (len == 0) {
		return 0;
	}

	if ( (s[0] < 'a' || s[0] > 'z') && (s[0] < 'A' || s[0] > 'Z') ) {
		return 0;
	}

	for (int i=1; i<len; i++) {
		char c = s[i];
		if ( (c < 'a' || c > 'z') && (c < 'A' || c > 'Z')
			    && (c != '_') && (c < '0' || c > '9')) {
			return 0;
		}
	}

	return 1;
}

int read_token (token *t) {
	char *s = t->str;
	char **endptr = NULL;
	t->type = NONE;

	//DEFUN
	if (strcmp(s, "defun") == 0) {
		t->type = DEFUN;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//RETURN
	if (strcmp(s, "return") == 0) {
		t->type = RETURN;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//PLUS
	if (strcmp(s, "+") == 0) {
		t->type = PLUS;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//MINUS
	if (strcmp(s, "-") == 0) {
		t->type = MINUS;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//MUL
	if (strcmp(s, "*") == 0) {
		t->type = MUL;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//DIV
	if (strcmp(s, "/") == 0) {
		t->type = DIV;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//MOD
	if (strcmp(s, "%") == 0) {
		t->type = MOD;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//AND
	if (strcmp(s, "and") == 0) {
		t->type = AND;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//OR
	if (strcmp(s, "or") == 0) {
		t->type = OR;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//NOT
	if (strcmp(s, "not") == 0) {
		t->type = NOT;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//LT
	if (strcmp(s, "lt") == 0) {
		t->type = LT;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//LE
	if (strcmp(s, "le") == 0) {
		t->type = LE;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//EQ
	if (strcmp(s, "eq") == 0) {
		t->type = EQ;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//GE
	if (strcmp(s, "ge") == 0) {
		t->type = GE;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//GT
	if (strcmp(s, "gt") == 0) {
		t->type = GT;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//IF
	if (strcmp(s, "if") == 0) {
		t->type = IF;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//ELSE
	if (strcmp(s, "else") == 0) {
		t->type = ELSE;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//ENDIF
	if (strcmp(s, "endif") == 0) {
		t->type = ENDIF;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//DROP
	if (strcmp(s, "drop") == 0) {
		t->type = DROP;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//SWAP
	if (strcmp(s, "swap") == 0) {
		t->type = SWAP;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//ROT
	if (strcmp(s, "rot") == 0) {
		t->type = ROT;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//ARG
	if (s[0] == 'a' && s[1] == 'r' && s[2] == 'g' && isdigit(s[3])) {
		int arg_no = strtol(s+3, endptr, 10);

		if (arg_no > 24) { 			//out of bounds
			t->type = BROKEN_TOKEN;
			t->literal_value = 0;
			t->arg_no = 0;
		} else {
			t->type = ARG;
			t->literal_value = 0;
			t->arg_no = num;
		}
	}

	if (t->type != NONE) return 0;


	//LITERAL

	//hexadecimal
	if (s[0] == '0' && s[1] == 'x' && isdigit(s[2])) {
		t->type = LITERAL;
		t->literal_value = strtol(s, endptr, 0);
		t->arg_no = 0;
	}

	//negative decimal
	else if (s[0] == '-' && isdigit(s[1])) {
		t->type = LITERAL;
		t->literal_value = strtol(s, endptr, 0);
		t->arg_no = 0;
	}

	//decimal
	else if (isdigit(s[0])) {
		t->type = LITERAL;
		t->literal_value = strtol(t->str, endptr, 0);
		t->arg_no = 0;
	}

	//IDENT / function name
	else if (valid_function_name(s)) {
		t->type = IDENT;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	//BROKEN TOKEN
	else {
		t->type = IDENT;
		t->literal_value = 0;
		t->arg_no = 0;
	}

	return 0;
}

const char *token_type_to_string (int type);

void print_token (token *t, FILE *out, int line, char *file_name) {
	if (t == NULL) return;
	printf("Type:%d,  String:%s,  Value:%d,  Arg:%d\n", t->type, t->str, t->literal_value, t->arg_no);

	char * str = t->str;
	if (t->type == DEFUN) {
		fprintf (out, ".FALIGN\n");
		defun = 1;
        free (t);
		return;
	}

	if (t->type == IDENT) {
		if (defun) {
			//create new function
			fprintf (out, "%s\n", str);
			fprintf (out, "\t;;prologue\n");
			//allocate 3 stack slots
			fprintf (out, "\tADD R6, R6, #-3\n");
			//save RA
			fprintf (out, "\tSTR R7, R6, #1\n");
			//save caller FP
			fprintf (out, "\tSTR R5, R6, #0\n");
			//setup local FP
			fprintf (out, "\tADD R5, R6, #0\n");
			fprintf (out, "\t;;function body\n");
		} else {
			//run function
			fprintf (out, "\tJSR %s\n", str);
		}	
	}

	if (t->type == RETURN) {
		fprintf (out, "\t;;function epilogue\n");
		//copy return value
		fprintf (out, "\tLDR R7, R6, #0\n"); //R5-2
		fprintf (out, "\tSTR R7, R5, #2\n");
		//free space for locals
		fprintf (out, "\tADD R6, R5, #0\n");
		//restore caller FP
		fprintf (out, "\tLDR R5, R6, #0\n");
		//restore RA
		fprintf (out, "\tLDR R7, R6, #1\n");
		//free space
		fprintf (out, "\tADD R6, R6, #2\n");
		//return
		fprintf (out, "\tRET\n");
	}

	if (t->type == PLUS) {
		//load operands
		fprintf(out, "\tLDR R1, R6, #0\n");
		fprintf(out, "\tLDR R2, R6, #1\n");
		//calculate
		fprintf(out, "\tADD R1, R1, R2\n");
		//spill R1 to stack, free it up
		fprintf(out, "\tSTR R1, R6, #1\n");
		fprintf(out, "\tADD R6, R6, #1\n");
	}

	if (t->type == MINUS) {
		//load operands
		fprintf(out, "\tLDR R1, R6, #0\n");
		fprintf(out, "\tLDR R2, R6, #1\n");
		//calculate
		fprintf(out, "\tSUB R1, R1, R2\n");
		//spill R1 to stack, free it up
		fprintf(out, "\tSTR R1, R6, #1\n");
		fprintf(out, "\tADD R6, R6, #1\n");
	}

	if (t->type == MUL) {
		//load operands
		fprintf(out, "\tLDR R1, R6, #0\n");
		fprintf(out, "\tLDR R2, R6, #1\n");
		//calculate
		fprintf(out, "\tMUL R1, R1, R2\n");
		//spill R1 to stack, free it up
		fprintf(out, "\tSTR R1, R6, #1\n");
		fprintf(out, "\tADD R6, R6, #1\n");
	}

	if (t->type == DIV) {
		//load operands
		fprintf(out, "\tLDR R1, R6, #0\n");
		fprintf(out, "\tLDR R2, R6, #1\n");
		//calculate
		fprintf(out, "\tDIV R1, R1, R2\n");
		//spill R1 to stack, free it up
		fprintf(out, "\tSTR R1, R6, #1\n");
		fprintf(out, "\tADD R6, R6, #1\n");
	}

	if (t->type == MOD) {
		//load operands
		fprintf(out, "\tLDR R1, R6, #0\n");
		fprintf(out, "\tLDR R2, R6, #1\n");
		//calculate
		fprintf(out, "\tMOD R1, R1, R2\n");
		//spill R1 to stack, free it up
		fprintf(out, "\tSTR R1, R6, #1\n");
		fprintf(out, "\tADD R6, R6, #1\n");
	}

	if (t->type == AND) {
		//load operands
		fprintf(out, "\tLDR R1, R6, #0\n");
		fprintf(out, "\tLDR R2, R6, #1\n");
		//calculate
		fprintf(out, "\tAND R1, R1, R2\n");
		//spill R1 to stack, free it up
		fprintf(out, "\tSTR R1, R6, #1\n");
		fprintf(out, "\tADD R6, R6, #1\n");
	}

	if (t->type == OR) {
		//load operands
		fprintf(out, "\tLDR R1, R6, #0\n");
		fprintf(out, "\tLDR R2, R6, #1\n");
		//calculate
		fprintf(out, "\tOR R1, R1, R2\n");
		//spill R1 to stack, free it up
		fprintf(out, "\tSTR R1, R6, #1\n");
		fprintf(out, "\tADD R6, R6, #1\n");
	}

	if (t->type == NOT) {
		//load operands
		fprintf(out, "\tLDR R1, R6, #0\n");
		//calculate
		fprintf(out, "\tNOT R1, R1\n");
		//spill R1 to stack, don't modify stack pointer
		fprintf(out, "\tSTR R1, R6, #0\n");
	}

	if (t->type == LT) {
		//load to registers
		fprintf(out, "\tLDR R1, R6, #0\n");
        fprintf(out, "\tLDR R2, R6, #1\n");
        //compare
        fprintf(out, "\tCMP R1, R2\n");
        //if true, go to body
        fprintf(out, "\tBRn %s_CMP_TRUE_%d\n", file_name, num);
        //else, set R0 to 0
        fprintf(out, "\tCONST R1, #0\n");
        fprintf(out, "\tJMP %s_CMP_END_%d\n", file_name, num);
        //if true, set R0 to 1
        fprintf(out, "%s_CMP_TRUE_%d\n", file_name, num);
		fprintf(out, "\tCONST R1, #1\n");
		//end
        fprintf(out, "%s_CMP_END_%d\n", file_name, num);
        //shift FP
        fprintf(out, "\tADD R6, R6, #1\n");
        //store to memory
        fprintf(out, "\tSTR R1, R6, #0\n");
        num++;
	}

	if (t->type == LE) {
		//load to registers
		fprintf(out, "\tLDR R1, R6, #0\n");
        fprintf(out, "\tLDR R2, R6, #1\n");
        //compare
        fprintf(out, "\tCMP R1, R2\n");
        //if true, go to body
        fprintf(out, "\tBRnz %s_CMP_TRUE_%d\n", file_name, num);
        //else, set R0 to 0
        fprintf(out, "\tCONST R1, #0\n");
        fprintf(out, "\tJMP %s_CMP_END_%d\n", file_name, num);
        //if true, set R0 to 1
        fprintf(out, "%s_CMP_TRUE_%d\n", file_name, num);
		fprintf(out, "\tCONST R1, #1\n");
		//end
        fprintf(out, "%s_CMP_END_%d\n", file_name, num);
        //shift FP
        fprintf(out, "\tADD R6, R6, #1\n");
        //store to memory
        fprintf(out, "\tSTR R1, R6, #0\n");
        num++;
	}

	if (t->type == EQ) {
		//load to registers
		fprintf(out, "\tLDR R1, R6, #0\n");
        fprintf(out, "\tLDR R2, R6, #1\n");
        //compare
        fprintf(out, "\tCMP R1, R2\n");
        //if true, go to body
        fprintf(out, "\tBRz %s_CMP_TRUE_%d\n", file_name, num);
        //else, set R0 to 0
        fprintf(out, "\tCONST R1, #0\n");
        fprintf(out, "\tJMP %s_CMP_END_%d\n", file_name, num);
        //if true, set R0 to 1
        fprintf(out, "%s_CMP_TRUE_%d\n", file_name, num);
		fprintf(out, "\tCONST R1, #1\n");
		//end
        fprintf(out, "%s_CMP_END_%d\n", file_name, num);
        //shift FP
        fprintf(out, "\tADD R6, R6, #1\n");
        //store to memory
        fprintf(out, "\tSTR R1, R6, #0\n");
        num++;
	}

	if (t->type == GE) {
		//load to registers
		fprintf(out, "\tLDR R1, R6, #0\n");
        fprintf(out, "\tLDR R2, R6, #1\n");
        //compare
        fprintf(out, "\tCMP R1, R2\n");
        //if true, go to body
        fprintf(out, "\tBRzp %s_CMP_TRUE_%d\n", file_name, num);
        //else, set R0 to 0
        fprintf(out, "\tCONST R1, #0\n");
        fprintf(out, "\tJMP %s_CMP_END_%d\n", file_name, num);
        //if true, set R0 to 1
        fprintf(out, "%s_CMP_TRUE_%d\n", file_name, num);
		fprintf(out, "\tCONST R1, #1\n");
		//end
        fprintf(out, "%s_CMP_END_%d\n", file_name, num);
        //shift FP
        fprintf(out, "\tADD R6, R6, #1\n");
        //store to memory
        fprintf(out, "\tSTR R1, R6, #0\n");
        num++;
	}

	if (t->type == GT) {
		//load to registers
		fprintf(out, "\tLDR R1, R6, #0\n");
        fprintf(out, "\tLDR R2, R6, #1\n");
        //compare
        fprintf(out, "\tCMP R1, R2\n");
        //if true, go to body
        fprintf(out, "\tBRp %s_CMP_TRUE_%d\n", file_name, num);
        //else, set R0 to 0
        fprintf(out, "\tCONST R1, #0\n");
        fprintf(out, "\tJMP %s_CMP_END_%d\n", file_name, num);
        //if true, set R0 to 1
        fprintf(out, "%s_CMP_TRUE_%d\n", file_name, num);
		fprintf(out, "\tCONST R1, #1\n");
		//end
        fprintf(out, "%s_CMP_END_%d\n", file_name, num);
        //shift FP
        fprintf(out, "\tADD R6, R6, #1\n");
        //store to memory
        fprintf(out, "\tSTR R1, R6, #0\n");
        num++;
	}

	if (t->type == IF) {
		//load boolean
		fprintf(out, "\tADD R6, R6, #1\n");
		//shift FP
        fprintf(out, "\tLDR R1, R6, #-1\n");

        max_if++; 
        last_ifs[max_if] = last_if;
        last_if = max_if;
        has_else = 0;

        fprintf(out, "%s_IF_%d\n", file_name, last_if);
        //else, branch to else clause
        fprintf(out, "\tBRz %s_ELSE_%d\n", file_name, last_if);
	}

	if (t->type == ELSE) {
		has_else = 1;
		fprintf(out, "\tJMP %s_ENDIF_%d\n", file_name, last_if);
        fprintf(out, "%s_ELSE_%d\n", file_name, last_if);
	}

	if (t->type == ENDIF) {
		if (!has_else) {
			fprintf(out, "%s_ELSE_%d\n", file_name, last_if);
		}
        fprintf(out, "%s_ENDIF_%d\n", file_name, last_if);
        
        last_if = last_ifs[last_if];
	}

	if (t->type == DROP) {
        //shift FP
        fprintf(out, "\tADD R6, R6, #1\n");
	}

	if (t->type == DUP) {
		//load to register
		fprintf(out, "\tLDR R1, R6, #0\n");
        //shift FP
        fprintf(out, "\tADD R6, R6, #-1\n");
        //store to memory
        fprintf(out, "\tSTR R1, R6, #0\n");
	}

	if (t->type == SWAP) {
		//load to registers
		fprintf(out, "\tLDR R1, R6, #0\n");
        fprintf(out, "\tLDR R2, R6, #1\n");
        //store to memory
        fprintf(out, "\tSTR R1, R6, #1\n");
        fprintf(out, "\tSTR R2, R6, #0\n");
	}

	if (t->type == ROT) {
		//load to registers
		fprintf(out, "\tLDR R1, R6, #0\n");
        fprintf(out, "\tLDR R2, R6, #1\n");
        fprintf(out, "\tLDR R3, R6, #2\n");
        //store to memory
        fprintf(out, "\tSTR R1, R6, #1\n");
        fprintf(out, "\tSTR R2, R6, #2\n");
        fprintf(out, "\tSTR R3, R6, #0\n");
	}

	if (t->type == ARG) {
		//load arg
		fprintf(out, "\tLDR R1, R5, #%d\n", t->arg_no + 2);
		//shift FP
		fprintf(out, "\tADD R6, R6, #-1\n");
		//store to stack
		fprintf(out, "\tSTR R1, R6, #0\n");
	}

	if (t->type == LITERAL) {
		int value = t->literal_value;
		short lower = (short)value & 0xFF;
		short upper = ((short)value >> 8) & 0xFF;
		//shift FP
		fprintf(out, "\tADD R6, R6, #-1\n");
		//store to stack
		fprintf(out, "\tCONST R1, #%d\n", lower);
		fprintf(out, "\tHICONST R1, #%d\n", upper);
		fprintf(out, "\tSTR R1, R6, #0\n");
	}

	//BROKEN
	defun = 0;
    free (t);
}

token * create_token (char * str, int len) {
	token *t;
	if (len < 1) printf("Length: %d\n", len);
	if (str == NULL) return NULL;
	if (strcmp(str,"\n") == 0) return NULL;
	
    t = (token*) malloc(sizeof(token));
    if (t == NULL) exit (1);
	
	int i;
	for (i=0; i<len; i++) {
		t->str[i] = str[i];
	}
	for (i=len; i<MAX_TOKEN_LENGTH; i++) {
		t->str[i] = '\0';
	}

	t->type = NONE;
	t->literal_value = 0;
	t->arg_no = 0;
	
	read_token(t);
	if (t->type == NONE) {
        free (t);
        return NULL;
    }
	return t;
}