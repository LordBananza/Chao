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

void nextToken() {
	if (token != NULL) {
		token = token->next;
	}
}

void expect(TokenType type) {

	nextToken();

	if (token->type != type) {
		printf("SYNTAX ERROR: %s was unexpected in the grammar\n", token->lexeme);
		exit(-1);
	} else {
		//printf("succeeded expect check\n");
	}

}



/**
 * Looks ahead by n tokens to determine what the next token will be; useful for predicting the path of the parser
 */
Type peek (int distance) {
	Token* peeker = token;



	for (int i = 0; i < distance; ++i) {
		if (peeker == NULL) {
			return END;
		}
		peeker = peeker->next;
	}

	if (peeker == NULL) {
		return END;
	}
	return peeker->type;

}



//Parsing Functions for every item in the CFG


void parseFunctionList();

void parseInstructionList();

void parseParameter() {
	expect(ID);
}

void parseParameterList() {
	parseParameter();

	if (peek(1) == ID) {
		expect(COMMA);
		parseParameterList();
	}
}

void parseArgument() {
	expect(TYPE);

	expect(ID);
}

//TODO Store data for various instruction types

void parseDeclaration() {
	expect(TYPE);

	expect(ID);

	if (peek(1) == EQUAL) {
		expect(EQUAL);

		expect(NUM);

		//printf("%s\n", token->lexeme);
	}

	expect(SEMICOLON);
}

void parseAssignment() {
	expect(ID);

	expect(EQUAL);

	if (peek(1) == ID) {
		expect(ID);
	} else {
		expect(NUM);
	}

	if (peek(1) == SEMICOLON) {
		expect(SEMICOLON);
		return;
	}

	expect(OPERATOR);

	if (peek(1) == ID) {
		expect(ID);
	} else {
		expect(NUM);
	}

	expect(SEMICOLON);
}

void parseCall() {
	expect(ID);

	expect(LPAREN);

	if (peek(1) != RPAREN) {
		parseParameterList();
	}

	expect(RPAREN);

	expect(SEMICOLON);
}

void parseComp() {
	if (peek(1) == NUM) {
		expect(NUM);
	} else {
		expect(ID);
	}

	expect(OPC);

	if (peek(1) == NUM) {
		expect(NUM);
	} else {
		expect(ID);
	}

}

void parseCompList() {
	parseComp();

	if (peek(1) == AND || peek(1) == OR) {
		parseCompList();
	}
}

void parseCondition() {
	parseCompList();
}

void parseIf() {
	expect(IF);

	expect(LPAREN);

	parseCondition();

	expect(RPAREN);

	expect(LBRACE);

	parseInstructionList();

	expect(RBRACE);
}

void parseWhile() {
	expect(WHILE);

	expect(LPAREN);

	parseCondition();
}

void parseInstruction() {

	//printf("i\n");

	if (peek(1) == TYPE) {
		parseDeclaration();
	} else if (peek(1) == ID) {
		if (peek(2) == EQUAL) {
			parseAssignment();
		} else if (peek(2) == LPAREN) {
			parseCall();
		}
	} else if (peek(1) == IF) {
		parseIf();
	} else if (peek(1) == WHILE) {
		parseWhile();
	}
}

void parseInstructionList() {
	//printf("il\n");

	parseInstruction();

	if (peek(1) == ID || peek(1) == TYPE || peek(1) == IF || peek(1) == WHILE) {
		parseInstructionList();
	}
}

void parseArgumentList() {
	//printf("al\n");

	if (peek(1) == RPAREN) {return;}

	parseArgument();

	if (peek(1) == COMMA) {
		expect(COMMA);
		parseArgumentList();
	}

}

void parseFunction() {
	//printf("f\n");
	expect(TYPE);

	expect(ID);

	expect(LPAREN);

	parseArgumentList();

	expect(RPAREN);

	expect(LBRACE);

	parseInstructionList();

	expect(RBRACE);

}


void parseFunctionList() {
	//printf("fl\n");

	parseFunction();
	if (peek(1) == TYPE) {
		parseFunctionList();
	}
}

//Macro function to recursively parse the tokens created by the lexer
Instruction* parseTokens(Token* head) {

	token = (Token*) malloc(sizeof(Token));
	token->next = head;

	lead = (Instruction*) malloc(sizeof(Instruction));
	node = lead;

	//TODO Implement recursive parser after defining CFG
	parseFunctionList();
	expect(END);

	return lead;
}
