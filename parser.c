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

char* getAddressString(char address) {
	char* addr = (char*) malloc(4);
	sprintf(addr, "%d", address);
	return addr;
}

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

	if (token->type == SLASH && token->next->type == STAR) {
		token = token->next;
			while (token->type != SLASH) {
				token = token->next;
				//printf("%d\n", token->type);
			}
			token = token->next;
			//printf("%d\n", token->type);
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
		if (peeker->type == SLASH && peeker->next->type == STAR) {
			peeker = peeker->next;
			while (peeker->type != SLASH) {
				peeker = peeker->next;
				//printf("%d\n", peeker->type);
			}
			peeker = peeker->next;
			//printf("%d (End of Peek)\n", peeker->type);
			
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
		//printf("Found a parameter with name %s and address %d\n", tracer->name, tracer->address);
		
		node->op1 = getOp("$", 1, getAddressString(tracer->address + tracer->index));
		newNode();
	
		tracer = tracer->next;
	}
}


//Parsing Functions for every item in the CFG


void parseFunctionList();

void parseInstructionList();

void parseParameter() {

	char offset = 0;

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
		
		offset = (char) atoi(token->lexeme);
		
		expect(RBRACKET);
	}
	
	IDName* id = findID(name);
	
	id->next = parameters;
	id->index = offset;
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
			
			node->type = "mov";
			node->tabCount = tabCount;
			node->op1 = strcat(getOp("#", 1, token->lexeme), ",");
			node->op2 = "acc";
			newNode();
			
			node->type = "st";
			node->op1 = getOp("$", 1, getAddressString(id->address));
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

	char mulOrDiv = 0;

	expect(ID);
	if (IDInUse(token->lexeme) == 0) {
			printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
			exit(-1);
	}
	
	//Stores where in memory the result will go
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
					
			} else if (peek(1) == ID) {
				expect(ID);
				if (IDInUse(token->lexeme) == 0) {
					printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
					exit(-1);
				}
				
				//TODO obtain value from the variable, then store it elsewhere to get the value out of the array
				
			}
			expect(RBRACKET);
			}
			
			node->type = "mov";
			node->tabCount = tabCount;
			node->op1 = strcat(getOp("#$", 2, getAddressString(address)), ",");
			node->op2 = "acc";
			newNode();
			
	} else {
		expect(NUM);
		
			
		node->type = "mov";
		node->tabCount = tabCount;
		node->op1 = strcat(getOp("#", 1, token->lexeme), ",");
		node->op2 = "acc";
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

	if (peek(1) == OPERATOR) {

	expect(OPERATOR);
	
	//Addition
	if (strcmp(token->lexeme, "+") == 0) {
		node->type = "add";
	
	//Subtraction
	} else if (strcmp(token->lexeme, "-") == 0) {
		node->type = "sub";
	}
	
	//TODO Put operands into ACC and C (first value) then B for second value
	} else if (peek(1) == STAR) {
		//printf("\nStar\n\n");
		
		expect(STAR);
		mulOrDiv = 1;
		
		
	} else if (peek(1) == SLASH) {
		//printf("\nSlash\n\n");
		
		expect(SLASH);
		mulOrDiv = 2;
	}

	if (peek(1) == ID) {
		expect(ID);
		if (IDInUse(token->lexeme) == 0) {
			printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
			exit(-1);
		}
	} else {
		expect(NUM);
		
		node->tabCount = tabCount;
		node->op1 = getOp("#", 1, token->lexeme);
		
		
		if (mulOrDiv == 0) {newNode();}
	}
	
	
	if (mulOrDiv != 0) {
		node->type = "mov";
		node->tabCount = tabCount;
		node->op1 = strcat(node->op1, ",");
		node->op2 = "b";
		newNode();
		
		if (mulOrDiv == 1) {
			node->type = "mul";
		} else {
			node->type = "div";
		}
		node->tabCount = tabCount;
		newNode();
		
		node->type = "ld";
		node->tabCount = tabCount;
		node->op1 = "b";
		newNode();
		
		
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

void parseReturn() {
	expect(RETURN);
	
	if (peek(1) == NUM) {
		expect(NUM);
		
		node->type = "mov";
		node->tabCount = tabCount;
		node->op1 = strcat(getOp("#", 1, token->lexeme), ",");
		node->op2 = "acc";
		newNode();
		
		node->type = "push";
		node->tabCount = tabCount;
		node->op1 = "acc";
		newNode();
		
		
	} else if (peek(1) == ID) {
		expect(ID);
		
		int address = (int) findID(token->lexeme)->address;
		
		//TODO finish getting address of variable;
		
		if (peek(1) == LBRACKET) {
			expect(LBRACKET);
			expect(NUM);
			expect(RBRACKET);
		}
	}
	
	node->type = "ret";
	node->tabCount = tabCount;
	newNode();
	
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
	} else if (peek(1) == RETURN) {
		parseReturn();
	} else {
		printf("Unexpected instruction beginning here: %s\n", token->next->lexeme);
	}
}

void parseInstructionList() {
	//printf("il\n");

	parseInstruction();

	if (peek(1) == ID || peek(1) == TYPE || peek(1) == IF || peek(1) == WHILE || peek(1) == ASM) {
		parseInstructionList();
	} else if (peek(1) == RETURN) {
		parseReturn();
		return;
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
	
	expect(TYPE);

	expect(ID);
	addID(token->lexeme, 0, 0);
	IDName* prevIDNames = IDNames;
	
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

void parseInclude() {
	char languageLibrary = 1;
	
	expect(POUND);
	expect(INCLUDE);
		
		//Include files with surrounded by " are user-generated
		//Include files surrounded by < are provided with the language
	if (peek(1) == QUOTE) {
		expect(QUOTE);
		languageLibrary = 0;
	} else {
		expect(OPC);
	}
		
	expect(ID);
		
	FILE* includeFile;
	if (languageLibrary == 1) {
		char fileName[1024];
		strncpy(fileName, "include/", 1024);
		includeFile = fopen(strcat(fileName, token->lexeme), "r");
		
	
	} else {
		includeFile = fopen(token->lexeme, "r");
	}
		
	if (!includeFile) {
		printf("ERROR: Header file %s not found\n", token->lexeme);
		exit(-1);
	} else {
		//store the token and instruction linked lists before recursing further
		Token* mainToken = token;
		Instruction* mainNode = lead;
		
		//recurse to parse the header file
		Token* includeToken = getAllTokens(includeFile);
		Instruction* includeInstruction = parseTokens(includeToken);
		
		//Attach the new instructions to the end of the current instruction list
		Instruction* tracer = mainNode;
		while (tracer->next != NULL) {
			tracer = tracer->next;
		}
		
		token = mainToken;
		lead = mainNode;
		tracer->next = includeInstruction;
	}
	
	if (languageLibrary == 0) {
		expect(QUOTE);
	} else {
		expect(OPC);
	}
	
}

void parseIncludeList() {

	parseInclude();
	
	if (peek(1) == POUND) {
		parseIncludeList();
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
	if (peek(1) == POUND) {parseIncludeList();}
	
	parseFunctionList();
	//printf("End of parser\n");
	expect(END);

	return lead;
}
