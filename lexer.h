/*
 * lexer.h
 *
 *  Created on: May 10, 2025
 *      Author: lordbananza
 */

#ifndef LEXER_H_
#define LEXER_H_

#include <stdio.h>

typedef enum TokenType {
	NUM, ID, EQUAL, ANDOR, SEMICOLON, OPERATOR, OPC, LPAREN, RPAREN, LBRACE, RBRACE, LBRACKET, RBRACKET, COMMA, TYPE, END, IF, WHILE, ASM, COMMENTSTART, COMMENTEND
} TokenType;

typedef struct Token {
	char lexeme[1024];
	enum TokenType type;
	struct Token* next;
} Token;


Token* getAllTokens(FILE* code);

void determineType(Token* token);

Token* getToken();




#endif /* LEXER_H_ */
