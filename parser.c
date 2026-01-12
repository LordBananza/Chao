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

int tabCount;

Token* token;

IDName* IDNames;

IDName* parameters;

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

IDName* findID (char* ID) {
	IDName* tracer = IDNames;
	
	while (tracer != NULL) {
		if (strcmp(tracer->name, ID) == 0) {return tracer;}
		tracer = tracer->next;
	}
	
	return NULL;
}


void newNode() {
	node->next = (Instruction*) malloc(sizeof(Instruction));
	node = node->next;
}

char* getOp(char* header, int headerSize, char* string) {
	char* op = (char*) malloc(sizeof(char)*1024);
	strncpy(op, header, 1024);
	strncpy(op + headerSize, string, 1024);
	return op;
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

void pushParameters() {
	IDName* tracer = parameters;
	
	while (tracer != NULL) {
		node->type = "push";
		node->tabCount = tabCount;
		char addr[3];
		int ad = (int) tracer->address;
		sprintf(addr, "%d", ad);
		
		//printf("Found a parameter with name %s and address %d\n", tracer->name, tracer->address);
		
		node->op1 = getOp("$", 1, addr);
		newNode();
	
		tracer = tracer->next;
	}
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
	char name[1024];
	strncpy(name, token->lexeme, 1024);
	
	if (peek(1) == LBRACKET) {
		
		//printf("%s %s\n", token->lexeme, name);
	
		expect(LBRACKET);
		expect(NUM);
		
		
		expect(RBRACKET);
	}
	
	IDName* id = findID(name);
	
	id->next = parameters;
	parameters = id;
}

void parseParameterList() {
	
	parseParameter();

	if (peek(1) == COMMA) {
		expect(COMMA);
		parseParameterList();
	}
}

void parseArgument() {
	expect(TYPE);

	expect(ID);
	
	addID(token->lexeme, 1, 1);
	
	node->type = "pop";
	node->tabCount = tabCount;
	char addr[3];
	int ad = (int) findID(token->lexeme)->address;
	sprintf(addr, "%d", ad);		
	node->op1 = getOp("$", 1, addr);
	newNode();
}

//TODO Store data for various instruction types

void parseDeclaration() {
	expect(TYPE);

	expect(ID);

	if (peek(1) == EQUAL) {
		addID(token->lexeme, 1, 1);
		IDName* id = findID(token->lexeme);
		
		expect(EQUAL);

		if (peek(1) == NUM) {
			expect(NUM);
			
			node->type = "and";
			node->op1 = "#0";
			node->tabCount = tabCount;
			newNode();
			
			node->type = "add";
			node->tabCount = tabCount;
			node->op1 = getOp("#", 1, token->lexeme);
			newNode();
			
			node->type = "st";
			char addr[3];
			int ad = (int) id->address;
			sprintf(addr, "%d", ad);
			node->op1 = getOp("$", 1, addr);
			node->tabCount = tabCount;
			newNode();
			
		} else {
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
	
	int destination = (int) findID(token->lexeme)->address;
	
	if (peek(1) == LBRACKET) {
		expect(LBRACKET);
		if (peek(1) == NUM) {
		
			expect(NUM);
			destination += atoi(token->lexeme);
			
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
		
		int address = (int) findID(token->lexeme)->address;
		
		if (IDInUse(token->lexeme) == 0) {
			printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
			exit(-1);
		}
		if (peek(1) == LBRACKET) {
			expect(LBRACKET);
			if (peek(1) == NUM) {
				expect(NUM);
				
				address += atoi(token->lexeme);
				
				node->type = "and";
				node->tabCount = tabCount;
				node->op1 = "#0";
				newNode();
			
				node->type = "add";
				node->tabCount = tabCount;
				char addr[4];
				sprintf(addr, "%d", address);
				node->op1 = getOp("#$", 2, addr);
				newNode();
					
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
		
		node->type = "and";
		node->tabCount = tabCount;
		node->op1 = "#0";
		newNode();
			
		node->type = "add";
		node->tabCount = tabCount;
		node->op1 = getOp("#", 1, token->lexeme);
		newNode();
			 
	}

	if (peek(1) == SEMICOLON) {
	
		expect(SEMICOLON);
		
		node->type = "st";
		node->tabCount = tabCount;
		char dest[4];
		sprintf(dest, "%d", destination);
		node->op1 = getOp("$", 1, dest);
		newNode();
		
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
	
	node->type = "st";
	node->tabCount = tabCount;
	char dest[4];
	sprintf(dest, "%d", destination);
	node->op1 = getOp("$", 1, dest);
	newNode();

	expect(SEMICOLON);
}

void parseCall() {
	expect(ID);
	
	char* name;
	strncpy(name, token->lexeme, 1024);
	
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
		parameters = NULL;
		parseParameterList();
		
		pushParameters();
		
		node->type = "call";
		node->tabCount = tabCount;
		node->op1 = name;
		newNode();
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
	node->type = token->lexeme;
	node->tabCount = tabCount;
	
	if (peek(1) == EQUAL) {
		expect(EQUAL);
		node->op1 = token->lexeme;
		
		expect(ID);
		node->op2 = token->lexeme;
		
	} else if (strcmp(token->lexeme, ".org") == 0) {
		node->tabCount = 0;
	} else {
		int currentOp = 1;
		while (peek(1) != RPAREN) {
			expect(ID);
			if(currentOp == 1) {
				node->op1 = token->lexeme;
				++currentOp;
			} else if(currentOp == 2) {
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
	node->type = strcat(token->lexeme, ":");
	node->tabCount = 0;
	tabCount = 1;
	
	node->next = (Instruction*) malloc(sizeof(Instruction));
	node = node->next;
	
	expect(LPAREN);

	parseArgumentList();

	expect(RPAREN);

	expect(LBRACE);

	parseInstructionList();

	expect(RBRACE);
	
	node->next = (Instruction*) malloc(sizeof(Instruction));
	node = node->next;
	
	//Restore scope to previous condition
	IDNames = prevIDNames;

}


void parseFunctionList() {
	//printf("fl\n");
	if (peek(3) == SEMICOLON) {parseDeclaration();}
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
	
	tabCount = 0;
	
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
