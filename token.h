/*
 * token.h
 */

#define MAX_TOKEN_LENGTH 250

#include <stdio.h>

typedef struct {
  enum { NONE, 		  //0
  		 DEFUN,       //1   start functions 	done1
  		 IDENT,       //2   function name 		done1
  		 RETURN,      //3						done1
         PLUS,        //4						done1
         MINUS,       //5						done1
         MUL,         //6						done1
         DIV,         //7						done1
         MOD,         //8						done1
         AND,         //9						done1
         OR,          //10						done1
         NOT,         //11						done1
         LT,          //12						done1
         LE,          //13						done1
         EQ,          //14						done1
         GE,          //15						done1
         GT,          //16						done1
         IF,          //17						done1
         ELSE,        //18						done1
         ENDIF,       //19						done1
         DROP,        //20						done1
         DUP,         //21						done1
         SWAP,        //22						done1
         ROT,         //23						done1
	 	 ARG,         //24  function argument	done
         LITERAL,     //25  integer done		done1
         BROKEN_TOKEN //26						done1
     } type;
  int literal_value;    // this field used to store the value of literal tokens
  int arg_no;           // this field used to store the index of argument literals argN
  char str[MAX_TOKEN_LENGTH];
} token;

int valid_function_name (char * s);

token * create_token(char * str, int len);

int read_token (token *theToken);

const char *token_type_to_string (int type);

void print_token (token *theToken, FILE *out, int line, char *file_name);

