/*
 * main.c
 *
 *  Created on: May 11, 2025
 *      Author: lordbananza
 */

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"


Token* frontToken;

Instruction* frontInstruction;





/**
 * The Chao compiler works much like a conventional compiler. It uses a lexer to separate the users' input into recognizable tokens
 * which are then given meaning by a parser. The parser then generates an output file of standard VMS assembly code.
 * If you are looking to write your own Chao code, check README.md on the Chao Github page:
 */
int main (int argc , const char* argv[]) {
	if (argc != 2) {
		printf("ERROR: please specify only the file to be compiled\n");
		exit(-1);
	}

	FILE *code = fopen(argv[1], "r");

	frontToken = getAllTokens(code);

	/*while (frontToken != NULL) {
			printf("%s type: %d\n", frontToken->lexeme, frontToken->type);
			frontToken = frontToken->next;
	}*/

	frontInstruction = parseTokens(frontToken);



	fclose(code);

}
