/*
 * lexer.c
 *
 *  Created on: May 10, 2025
 *      Author: lordbananza
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lexer.h"

//Pointers to keep track of crucial points in linked list of Tokens (last and first)
struct Token* token;
struct Token* head;

/**
 * Based on the lexeme of the token, determines what type it must be
 */
void determineType (Token* token) {
	if (strcmp(token->lexeme, "int") == 0) {
		token->type = INT;
		return;
	} else if (strcmp(token->lexeme, "")) {
		token->type = END;
		return;
	} else {
		token->type = ID;
		return;
	}
}

/**
 * Creates a linked list of Tokens based on the text file given to the program upon launch.
 * Most Tokens are separated by spaces, newlines, etc. However, not all Tokens are (like the
 * Token for a semicolon).
 */
void getAllTokens(FILE *code) {
	token = (struct Token*) malloc(sizeof(Token));
	head = token;

	char line[1024];

	fgets(line, 1024, code);

	while(!feof(code)) {


		char tracer = line[0];
		int i = 0;
		int j = 0;

		while (tracer != '\0') {
			if (tracer == ' ' || tracer == '\n') {

				++i;
				tracer = line[i];
				determineType(token);
				token->next = (Token*) malloc(sizeof(Token));
				token = token->next;
				j = 0;

			} else {

				token->lexeme[j] = tracer;
				++i;
				++j;
				tracer = line[i];

			}
		}
		fgets(line, 1024, code);
	}

	determineType(token);

}

/**
 * Fetches the next token from the linked list and updates the head node to be the next unread token.
 */

Token* getToken () {
	Token* result = head;
	if (head != NULL) {head = head->next;}
	return result;
}

