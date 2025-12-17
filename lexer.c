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
struct Token* currToken;
struct Token* head;

/**
 * Based on the lexeme of the token, determines what type it must be
 */
void determineType (Token* token) {
	//printf("currToken = %s\n", token->lexeme);
	if (strcmp(token->lexeme, "char") == 0 || strcmp(token->lexeme, "char*") == 0) {
		token->type = TYPE;
		//printf("int8\n");
		return;

	} else if (strcmp(token->lexeme, "asm") == 0) {
		token->type = ASM;
		//printf("int8\n");
		return;

	} else if (strcmp(token->lexeme, "/") == 0) {
		token->type = SLASH;
		//printf("int8\n");
		return;

	} else if (strcmp(token->lexeme, "*") == 0) {
		token->type = STAR;
		//printf("int8\n");
		return;

	} else if (strcmp(token->lexeme, "while") == 0) {
		token->type = WHILE;
		return;

	} else if (strcmp(token->lexeme, "") == 0 || strcmp(token->lexeme, "\0") == 0) {
		token->type = END;
		return;

	} else if (strcmp(token->lexeme, "if") == 0) {
		token->type = IF;
		return;

	} else if (strcmp(token->lexeme, "=") == 0) {
		//printf("equal\n");
		token->type = EQUAL;
		return;

	} else if (strcmp(token->lexeme, ";") == 0) {
		//printf("semicolon\n");
		token->type = SEMICOLON;
		return;

	} else if (strcmp(token->lexeme, "(") == 0) {
		token->type = LPAREN;
		return;

	} else if (strcmp(token->lexeme, ")") == 0) {
		token->type = RPAREN;
		return;

	} else if (strcmp(token->lexeme, "{") == 0) {
		token->type = LBRACE;
		return;

	} else if (strcmp(token->lexeme, "}") == 0) {
		token->type = RBRACE;
		return;

	}  else if (strcmp(token->lexeme, "[") == 0) {
		token->type = LBRACKET;
		return;

	} else if (strcmp(token->lexeme, "]") == 0) {
		token->type = RBRACKET;
		return;

	} else if (strcmp(token->lexeme, ",") == 0) {
		token->type = COMMA;
		return;

	} else if (strcmp(token->lexeme, "&&") == 0 || strcmp(token->lexeme, "||") == 0) {
		token->type = ANDOR;
		return;

	} else if (strcmp(token->lexeme, "+") == 0 || strcmp(token->lexeme, "-") == 0 || strcmp(token->lexeme, "*") == 0 ||
			strcmp(token->lexeme, "/") == 0 || strcmp(token->lexeme, "<<") == 0 || strcmp(token->lexeme, ">>") == 0 ||
			strcmp(token->lexeme, "&") == 0 || strcmp(token->lexeme, "|") == 0 || strcmp(token->lexeme, "^") == 0 ||
			strcmp(token->lexeme, "~") == 0) {
		token->type = OPERATOR;
		return;

	} else if (strcmp(token->lexeme, "<") == 0 || strcmp(token->lexeme, "<=") == 0 || strcmp(token->lexeme, ">") == 0 ||
			strcmp(token->lexeme, ">=") == 0 || strcmp(token->lexeme, "==") == 0 || strcmp(token->lexeme, "!=") == 0) {
		token->type = OPC;
		return;

	} else if (strspn(token->lexeme, "0123456789") != 0) {
		token->type = NUM;
		//printf("here\n");
		return;
	}

	else {
		token->type = ID;
		//printf("ID with name %s\n", token->lexeme);
		return;
	}
}

/**
 * Creates a linked list of Tokens based on the text file given to the program upon launch.
 * Most Tokens are separated by spaces, newlines, etc. However, not all Tokens are (like the
 * Token for a semicolon).
 */
Token* getAllTokens(FILE *code) {
	currToken = (struct Token*) malloc(sizeof(Token));
	head = currToken;

	char line[1024];

	fgets(line, 1024, code);

	while(!feof(code)) {


		char tracer = line[0];
		int i = 0;
		int j = 0;


		while (tracer != '\0') {
		

			if (tracer == ' ' || tracer == ';' || tracer == '=' || tracer == '(' || tracer == ')' || tracer == '{' || tracer == '}' || tracer == '[' || tracer == ']' || tracer == ',' || tracer == '&' || tracer == '|' || tracer == '^' || tracer == '/' || tracer == '*' || tracer == '~' || tracer == '\n') {


				if (strcmp(currToken->lexeme, "") != 0 && tracer != '&' && tracer != '|' && tracer != '!' && tracer != '=') {

				determineType(currToken);
				currToken->next = (Token*) malloc(sizeof(Token));
				currToken = currToken->next;
				}

				if (tracer == ';' || tracer == '(' || tracer == ')' || tracer == '{' || tracer == '}' || tracer == ',' || tracer == '[' || tracer == ']' ) {
					currToken->lexeme[0] = tracer;
					determineType(currToken);
					currToken->next = (Token*) malloc(sizeof(Token));
					currToken = currToken->next;


				} else if (tracer == '&' || tracer == '|' || tracer == '='|| tracer == '!' || tracer == '/' || tracer == '*') {

					if (strcmp(currToken->lexeme, "&") == 0 && tracer == '&') {
						currToken->lexeme[1] = '&';
						j = 2;
						//printf("there\n");
					} else if (strcmp(currToken->lexeme, "|") == 0 && tracer == '|') {
						currToken->lexeme[1] = '|';
						j = 2;
						//printf("everywhere\n");
					} else if (strcmp(currToken->lexeme, "=") == 0 && tracer == '=') {
						currToken->lexeme[1] = '=';
						j = 2;
						//printf("everywhere\n");
					} else if (strcmp(currToken->lexeme, "!") == 0 && tracer == '=') {
						currToken->lexeme[1] = '=';
						j = 2;
						//printf("everywhere\n");
					} else {
						//printf("here %s %c\n", currToken->lexeme, tracer);
						if (strcmp(currToken->lexeme, "") != 0) {
						determineType(currToken);
						currToken->next = (Token*) malloc(sizeof(Token));
						currToken = currToken->next;
						currToken->lexeme[0] = tracer;
						//printf(" %s\n", currToken->lexeme);
						j = 1;
						} else {
							currToken->lexeme[0] = tracer;
						}
					}
				}

				++i;
				tracer = line[i];
				//printf("%d\n", tracer);

				j = 0;

			} else {

				currToken->lexeme[j] = tracer;
				++i;
				++j;
				tracer = line[i];
				//printf("%s\n", currToken->lexeme);

			}
		}
		
		fgets(line, 1024, code);
	}
	
	currToken->lexeme[0] = '\0';
	determineType(currToken);

	return head;

}

/**
 * Fetches the next token from the linked list and updates the head node to be the next unread token.
 */

Token* getToken () {
	Token* result = head;
	if (head != NULL) {head = head->next;}
	return result;
}

