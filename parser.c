/*
 * parser.c
 *
 *  Created on: May 11, 2025
 *      Author: lordbananza
 */
#include <stdlib.h>
#include "parser.h"
#include <string.h>

Instruction* node;

Instruction* lead;

Token* token;

IDName* IDNames;

//Checks to see if an ID is already in use elsewhere in the scope of the code
char IDInUse(char* id) {
	IDName* tracer = IDNames;
	while (tracer != NULL) {
		if (strcmp(tracer->name, id) == 0) {return 1;}
		tracer = tracer->next;
	}
	return 0;

}

void addID(char* id) {
	if (IDInUse(id) == 0) {
		IDName* newID = (struct IDName*) malloc(sizeof(IDName));
		strncpy(newID->name, token->lexeme, 1024);
		newID->next = IDNames;
		IDNames = newID;
	} else {
		printf("ERROR: variable name \"%s\" is used to name more than one variable/function\n", id);
		exit(-1);
	}
}


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
		printf("SYNTAX ERROR: %s of type %d was unexpected in the grammar. A token of type %d was expected\n", token->lexeme, token->type, type);
		exit(-1);
	} else {
		//printf("succeeded expect check\n");
	}

}



/**
 * Looks ahead by n tokens to determine what the next token will be; useful for predicting the path of the parser
 */
TokenType peek (int distance) {
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
	addID(token->lexeme);
}

//TODO Store data for various instruction types

void parseDeclaration() {
	expect(TYPE);

	expect(ID);
	addID(token->lexeme);

	if (peek(1) == EQUAL) {
		expect(EQUAL);

		if (peek(1) == NUM) {expect(NUM);}
		else {expect(ID);}

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
	//TODO Allow for scoping of variables by separating IDNames within each
	// function/set of brackets from those outside of it
	
	expect(TYPE);

	expect(ID);
	addID(token->lexeme);

	expect(LPAREN);

	parseArgumentList();

	expect(RPAREN);

	expect(LBRACE);

	parseInstructionList();

	expect(RBRACE);

}


void parseFunctionList() {
	//printf("fl\n");
	if (peek(3) == EQUAL) {parseDeclaration();}
	else {parseFunction();}
	
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
	
	IDNames = (IDName*) malloc(sizeof(IDName));

	//printf("Start of parser\n");
	parseFunctionList();
	//printf("End of parser\n");
	expect(END);

	return lead;
}
