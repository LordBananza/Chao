/*
 * parser.c
 *
 *  Created on: May 11, 2025
 *      Author: lordbananza
 */
#include <stdlib.h>
#include "parser.h"

Instruction* node;

Instruction* lead;

Token* token;

/**
 * Confirms whether the specified token is of the expected type, according to the grammar of Chao.
 */
void expect(TokenType type) {

	if (token->type != type) {
		printf("SYNTAX ERROR: %s was unexpected in the grammar\n", token->lexeme);
		exit(-1);
	} else {
		//printf("succeeded expect check\n");
	}

	token = token->next;

}

/**
 * Looks ahead by n tokens to determine what the next token will be; useful for predicting the path of the parser
 */
Type peek (int distance) {
	Token* peeker = token;
	for (int i = 0; i < distance-1; ++i) {
		peeker = peeker->next;
	}

	return peeker->type;

}



//Parsing Functions for every item in the CFG


void parseFunctionList();


void parseArgument() {
	expect(TYPE);

	expect(ID);
}

void parseArgumentList() {

	if (peek(1) == RPAREN) {return;}

	parseArgument();

	if (peek(1) == COMMA) {
		expect(COMMA);
		parseArgumentList();
	}

}

void parseFunction() {
	expect(TYPE);

	expect(ID);

	expect(LPAREN);

	parseArgumentList();

	expect(RPAREN);

	expect(LBRACE);

	//TODO Continue from here
	//parseInstructionList();

	expect(RBRACE);

}


void parseFunctionList() {
	parseFunction();
	if (peek(1) == TYPE) {
		parseFunctionList();
	}
}

//Macro function to recursively parse the tokens created by the lexer
Instruction* parseTokens(Token* head) {

	token = head;

	//TODO Implement recursive parser after defining CFG
	parseFunctionList();
	expect(END);

	return lead;
}
