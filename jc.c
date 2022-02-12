#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "token.h"
#include "LC4.h"
#define MEMORY_SIZE 65536
#define MAX_TOKEN_LENGTH 250

FILE * in_file;
FILE * out_file;
char * in_file_name;
int length = 0;

char bin_buffer[17];
char token_buffer[MAX_TOKEN_LENGTH];

void read_file() {
	int line = 1;
	in_file = fopen (in_file_name, "r");


	char file_name[length - 2];
	for (int i=0; i < length - 2; i++) { //copies up to before "."
		file_name[i] = in_file_name[i];
	}

	if (in_file == NULL) {
		printf ("Couldn't open file\n");
        fclose (out_file);
		exit(1);
	}

	char c;
	int len = 0;
	int comment = 0;
	token * t;

	//read token
	while ((c = fgetc(in_file)) != EOF) {
		if (c == '\n' || c == '\r') {
			if (len > 0) {
				t = create_token(token_buffer, len);
				print_token(t, out_file, line, file_name);
				len = 0;
				line++;
			}

			comment = 0;
		} else if (!comment) {			
			if (c == ' ' || c == '\t') {
				if (len > 0) {
					t = create_token(token_buffer, len);
					print_token(t, out_file, line, file_name);
					len = 0;
				}
			} else if (c == ';') {
				comment = 1;
				len = 0;
			} else {
				token_buffer[len] = c;
				len++;
			}
		}
	}
	//at EOF
	if (len > 0) {
		t = create_token(token_buffer, len);
	    print_token(t, out_file, line, file_name);
	}

	fclose (in_file);
	fclose (out_file);
}

void set_up_output() {
	length = strlen(in_file_name);
	char out_file_name[length + 2];
	
	for (int i=0; i < length - 2; i++) { //copies up to before "."
		out_file_name[i] = in_file_name[i];
	}

	//add asm extension
	out_file_name[length - 2] = '.';
	out_file_name[length - 1] = 'a';
	out_file_name[length] = 's';
	out_file_name[length + 1] = 'm';
	out_file_name[length + 2] = '\0';

	out_file = fopen(out_file_name, "w");

	fprintf(out_file, ".CODE\n");
}

char *short_to_bin_byte (short a) {
    for (int i = 15; i >= 0; i--) {
        if ((a & 0x01) == 0x01) {
        	bin_buffer[i] = '1';
        } else {
        	bin_buffer[i] = '0';
        }

        a >>= 1;
    }
    return bin_buffer;
}

int main (int arc, char **argv) {
	in_file_name = argv[1];
	set_up_output();

	//read all .obj files to memory
	read_file();
}