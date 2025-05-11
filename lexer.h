/*
 * lexer.h
 *
 *  Created on: May 10, 2025
 *      Author: lordbananza
 */

#ifndef LEXER_H_
#define LEXER_H_

typedef enum TokenType {
	INT, ID, PLUS, END, MINUS
} TokenType;

typedef struct Token {
	char lexeme[1024];
	enum TokenType type;
	struct Token* next;
} Token;


void getAllTokens(FILE* code);

void determineType(Token* token);

Token* getToken();




#endif /* LEXER_H_ */
