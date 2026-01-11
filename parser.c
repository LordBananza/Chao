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

//0 = free; 1 = taken;
char addresses[255];


void freeMemory(char address, int size) {
	for (int i = 0; i < size; ++i) {
		addresses[address+i] = 0;
	}
}

char findFreeMemory(int size) {
	char ready = -1;
	unsigned char start = 0;
	int count = 0;
	for (int i = 0; i < 255; ++i) {
		if (ready == -1) {
			if (addresses[i] == 0) {
				printf("Found potential starting point at %d\n", i);
				start = i;
				ready = 1;
				count = 1;
				addresses[i] = 1;
			}
		} else {
			if (count == size) {printf("Found free memory at address %d\n", start); return start;}
			
			if (addresses[i] == 0) {
				++count;
				addresses[i] = 1;
				
			} else {
				count = 0;
				ready = -1;
			}
		}
	}
}


//Checks to see if an ID is already in use elsewhere in the scope of the code
char IDInUse(char* id) {
	IDName* tracer = IDNames;
	while (tracer != NULL) {
		if (strcmp(tracer->name, id) == 0) {return 1;}
		tracer = tracer->next;
	}
	return 0;

}

void addID(char* id, char variable, char size) {
	if (IDInUse(id) == 0) {
		IDName* newID = (struct IDName*) malloc(sizeof(IDName));
		strncpy(newID->name, id, 1024);
		
		if (variable == 1) {
			newID->variable = 1;
			newID->address = findFreeMemory(size);
		}
		
		printf("Added new variable/function named \'%s\"\n", newID->name);
		
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

	if (token->type == SLASH) {
		token = token->next;
			while (token->type != SLASH) {
				token = token->next;
			}
			token = token->next;
		}

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
		peeker = peeker->next;
		if (peeker->type == SLASH) {
			peeker = peeker->next;
			while (peeker->type != SLASH) {
				peeker = peeker->next;
			}
			peeker = peeker->next;
		}
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
	if (IDInUse(token->lexeme) == 0) {
		printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
		exit(-1);
	}
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
	addID(token->lexeme, 1, 1);
}

//TODO Store data for various instruction types

void parseDeclaration() {
	expect(TYPE);

	expect(ID);

	if (peek(1) == EQUAL) {
	
		addID(token->lexeme, 1, 1);
		
		expect(EQUAL);

		if (peek(1) == NUM) {expect(NUM);}
		else {
			expect(ID);
			if (IDInUse(token->lexeme) == 0) {
				printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
				exit(-1);
			}
			
		}
		
		

		//printf("%s\n", token->lexeme);
	} else if (peek(1) == LBRACKET) {
		char name[1024];
		strncpy(name, token->lexeme, 1024);
		//printf("%s %s\n", token->lexeme, name);
	
		expect(LBRACKET);
		expect(NUM);
		
		addID(name, 1, atoi(token->lexeme));
		
		//printf("%s\n", token->lexeme);
		//printf("%d\n", atoi(token->lexeme));
		
		expect(RBRACKET);
	} else {
		addID(token->lexeme, 1, 1);
	}

	expect(SEMICOLON);
}

void parseAssignment() {

	expect(ID);
	if (IDInUse(token->lexeme) == 0) {
			printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
			exit(-1);
	}
	
	if (peek(1) == LBRACKET) {
		expect(LBRACKET);
		if (peek(1) == NUM) {
			expect(NUM);
		} else if (peek(1) == ID) {
			expect(ID);
			if (IDInUse(token->lexeme) == 0) {
				printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
				exit(-1);
			}
		}
		expect(RBRACKET);
	}

	expect(EQUAL);

	if (peek(1) == ID) {
		expect(ID);
		if (IDInUse(token->lexeme) == 0) {
			printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
			exit(-1);
		}
		if (peek(1) == LBRACKET) {
			expect(LBRACKET);
			if (peek(1) == NUM) {
				expect(NUM);
			} else if (peek(1) == ID) {
				expect(ID);
				if (IDInUse(token->lexeme) == 0) {
					printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
					exit(-1);
				}
			}
			expect(RBRACKET);
			}
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
		if (IDInUse(token->lexeme) == 0) {
			printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
			exit(-1);
		}
	} else {
		expect(NUM);
	}

	expect(SEMICOLON);
}

void parseCall() {
	expect(ID);
	if (IDInUse(token->lexeme) == 0) {
		printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
		exit(-1);
	}
	if (peek(1) == LBRACKET) {
			expect(LBRACKET);
			if (peek(1) == NUM) {
				expect(NUM);
			} else if (peek(1) == ID) {
				expect(ID);
				if (IDInUse(token->lexeme) == 0) {
					printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
					exit(-1);
				}
			}
			expect(RBRACKET);
			}

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
		if (IDInUse(token->lexeme) == 0) {
			printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
			exit(-1);
		}
		if (peek(1) == LBRACKET) {
			expect(LBRACKET);
			if (peek(1) == NUM) {
				expect(NUM);
			} else if (peek(1) == ID) {
				expect(ID);
				if (IDInUse(token->lexeme) == 0) {
					printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
					exit(-1);
				}
			}
			expect(RBRACKET);
			}
	}

	if (peek(1) == OPC) {
	expect(OPC);

	if (peek(1) == NUM) {
		expect(NUM);
	} else {
		printf("Here\n");
		expect(ID);
		if (IDInUse(token->lexeme) == 0) {
			printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
			exit(-1);
		}
		if (peek(1) == LBRACKET) {
			expect(LBRACKET);
			if (peek(1) == NUM) {
				expect(NUM);
			} else if (peek(1) == ID) {
				expect(ID);
				if (IDInUse(token->lexeme) == 0) {
					printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
					exit(-1);
				}
			}
			expect(RBRACKET);
			}
	}
	}

}

void parseCompList() {
	parseComp();

	if (peek(1) == ANDOR) {
		expect(ANDOR);
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
	IDName* prevIDNames = IDNames;

	parseInstructionList();
	IDNames = prevIDNames;

	expect(RBRACE);
}

void parseWhile() {
	expect(WHILE);
	expect(LPAREN);

	parseCondition();
	
	expect(RPAREN);
	expect(LBRACE);
	IDName* prevIDNames = IDNames;
	
	parseInstructionList();
	IDNames = prevIDNames;
	
	expect(RBRACE);
}

void parseASM() {
	expect(ASM);
	
	expect(LPAREN);
	
	//Instruction
	expect(ID);
	
	if (peek(1) == EQUAL) {
		expect(EQUAL);
		
		expect(ID);
		
	} else if (strcmp(token->lexeme, ".org") == 0) {
	
	} else {
		strncpy(node->type, token->lexeme, 5);
		int currentOp = 1;
		while (peek(1) != RPAREN) {
			expect(ID);
			if(currentOp == 1) {
				node->op1 = token->lexeme;
				++currentOp;
			} else if(currentOp == 1) {
				node->op2 = token->lexeme;
				++currentOp;
			} else {
				node->op3 = token->lexeme;
			}
		}
	}
	
	expect(RPAREN);
	
	node->next = (Instruction*) malloc(sizeof(Instruction));
	node = node->next;
	
	expect(SEMICOLON);
}

void parseInstruction() {

	//printf("i\n");
	

	if (peek(1) == TYPE) {
		parseDeclaration();
	} else if (peek(1) == ID) {
		if (peek(2) == LPAREN) {
			parseCall();
		} else {
			parseAssignment();
		}
	} else if (peek(1) == IF) {
		parseIf();
	} else if (peek(1) == WHILE) {
		parseWhile();
	} else if (peek(1) == ASM) {
		parseASM();
	} else {
		printf("Unexpected instruction beginning here: %s\n", token->next->lexeme);
	}
}

void parseInstructionList() {
	//printf("il\n");

	parseInstruction();

	if (peek(1) == ID || peek(1) == TYPE || peek(1) == IF || peek(1) == WHILE || peek(1) == ASM) {
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
	
	//Allows for scoping of variables by separating IDNames within each
	//function/set of brackets from those outside of it
	IDName* prevIDNames = IDNames;
	
	expect(TYPE);

	expect(ID);
	addID(token->lexeme, 0, 0);
	
	expect(LPAREN);

	parseArgumentList();

	expect(RPAREN);

	expect(LBRACE);

	parseInstructionList();

	expect(RBRACE);
	
	//Restore scope to previous condition
	IDNames = prevIDNames;

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
	for (int i = 0; i < 255; ++i) {
		addresses[i] = 0;
	}

	//printf("Start of parser\n");
	parseFunctionList();
	//printf("End of parser\n");
	expect(END);

	return lead;
}
