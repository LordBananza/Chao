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

int ifNumber;
int whileNumber;
char ifOrWhile;

char* getAddressString(char address) {
	char* addr = (char*) malloc(4);
	sprintf(addr, "%x", address);
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
	sprintf(addr, "%x", ad);		
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
			node->op1 = getOp("#", 1, strcat(token->lexeme, ","));
			node->op2 = "acc";
			newNode();
			
			node->type = "st";
			node->op1 = getOp("$", 1, getAddressString(id->address));
			node->tabCount = tabCount;
			newNode();
			
		} else {
		expect(ID);
		
		int address = (int) findID(token->lexeme)->address;
		char op1Offset = 0;
		char* offsetAddress;
		
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
				
				op1Offset = 1;
				offsetAddress = token->lexeme;
			
		}
				
			expect(RBRACKET);
			}
			
			if (op1Offset == 1) {
			//The compiler knows the array's starting address. We'll add that as a constant value to the unknown offset
			
			//load the variable for offset
			node->type = "ld";
			node->tabCount = tabCount;
			node->op1 = getOp("$", 1, getAddressString(findID(offsetAddress)->address));
			newNode();
			
			
			//store offset in c register
			node->type = "st";
			node->tabCount = tabCount;
			node->op1 = "c";
			newNode();
			
			//load the value of the offset plus the known address
			node->type = "ld";
			node->tabCount = tabCount;
			char addr[4];
			node->op1 = "c";
			sprintf(addr, "%x", address);
			node->op2 = "+";
			node->op3 = getOp("$", 1, addr);
			newNode();
			
		} else {
			
			node->type = "ld";
			node->tabCount = tabCount;
			node->op1 = getOp("$", 1, getAddressString(address));
			newNode();
			
		}
			
			node->type = "st";
			node->op1 = getOp("$", 1, getAddressString(id->address));
			node->tabCount = tabCount;
			newNode();
			
			//TODO Add array checks and instructions
			
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
	char arrOffset = 0;
	char* offsetID;
	
	
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
			
			//Obtain the value for 
			offsetID = token->lexeme;
			arrOffset = 1;
		}
		expect(RBRACKET);
	}

	expect(EQUAL);

	if (peek(1) == ID) {
		expect(ID);
		
		int address = (int) findID(token->lexeme)->address;
		char op1Offset = 0;
		char* offsetAddress;
		
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
				
				op1Offset = 1;
				offsetAddress = token->lexeme;
			
		}
				
			expect(RBRACKET);
			}
			
			if (op1Offset == 1) {
			//The compiler knows the array's starting address. We'll add that as a constant value to the unknown offset
			
			//load the variable for offset
			node->type = "ld";
			node->tabCount = tabCount;
			node->op1 = getOp("$", 1, getAddressString(findID(offsetAddress)->address));
			newNode();
			
			
			//store offset in c register
			node->type = "st";
			node->tabCount = tabCount;
			node->op1 = "c";
			newNode();
			
			//load the value of the offset plus the known address
			node->type = "ld";
			node->tabCount = tabCount;
			char addr[4];
			node->op1 = "c";
			sprintf(addr, "%x", address);
			node->op2 = "+";
			node->op3 = getOp("$", 1, addr);
			newNode();
			
		} else {
			
			node->type = "ld";
			node->tabCount = tabCount;
			node->op1 = getOp("$", 1, getAddressString(address));
			newNode();
			
		}
			
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
		
		//if the index of the destination array is determined by a variable
		if (arrOffset == 1) {
			//keep the result in b register
			node->type = "st";
			node->tabCount = tabCount;
			node->op1 = "b";
			newNode();
			
			//load the variable offset
			node->type = "ld";
			node->tabCount = tabCount;
			node->op1 = getOp("$", 1, getAddressString(findID(offsetID)->address));
			newNode();
			
			//store offset in c register
			node->type = "st";
			node->tabCount = tabCount;
			node->op1 = "c";
			newNode();
			
			
			//load the result back into acc
			node->type = "ld";
			node->tabCount = tabCount;
			node->op1 = "b";
			newNode();
			
			//store the result to the offset plus the known address
			node->type = "st";
			node->tabCount = tabCount;
			char addr[4];
			node->op1 = "c";
			sprintf(addr, "%x", destination);
			node->op2 = "+";
			node->op3 = getOp("$", 1, addr);
			newNode();
			
		} else {
		
		node->type = "st";
		node->tabCount = tabCount;
		char dest[4];
		sprintf(dest, "%x", destination);
		node->op1 = getOp("$", 1, dest);
		newNode();
		
		}
		
		return;
	}

	char* optype;
	char* op2;

	if (peek(1) == OPERATOR) {

	expect(OPERATOR);
	
	
	
	//Addition
	if (strcmp(token->lexeme, "+") == 0) {
		optype = "add";
	
	//Subtraction
	} else if (strcmp(token->lexeme, "-") == 0) {
		optype = "sub";
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
		
		int address = (int) findID(token->lexeme)->address;
		char op2Offset = 0;
		char* offsetAddress;
		
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
				
				op2Offset = 1;
				offsetAddress = token->lexeme;
			
		}
				
			expect(RBRACKET);
			}
			
			if (op2Offset == 1) {
			
			//store first operand in b register
			node->type = "st";
			node->tabCount = tabCount;
			node->op1 = "b";
			newNode();
			
			
			//load the variable offset
			node->type = "ld";
			node->tabCount = tabCount;
			node->op1 = getOp("$", 1, getAddressString(findID(offsetAddress)->address));
			newNode();
			
			//store address in c register
			node->type = "st";
			node->tabCount = tabCount;
			node->op1 = "c";
			newNode();
			
			//load the value of the offset plus the known address
			node->type = "ld";
			node->tabCount = tabCount;
			char addr[4];
			node->op1 = "c";
			sprintf(addr, "%x", address);
			node->op2 = "+";
			node->op3 = getOp("$", 1, addr);
			newNode();
			
			//store value in c register
			node->type = "st";
			node->tabCount = tabCount;
			node->op1 = "c";
			newNode();
			
			//load original operand back into acc
			node->type = "ld";
			node->tabCount = tabCount;
			node->op1 = "b";
			newNode();
			
			op2 = "$c";
			
		} else {
			
			op2 = getOp("#$", 2, getAddressString(address));
			
		}
			
	} else {
		expect(NUM);
		
		node->tabCount = tabCount;
		op2 = getOp("#", 1, token->lexeme);
		
		
	}
	
	
	if (mulOrDiv != 0) {
	
		//store the acc value in c (lower 8 bits of first operand)
		node->type = "st";
		node->tabCount = tabCount;
		node->op1 = "c"; 
		newNode();
	
		node->type = "ld";
		node->tabCount = tabCount;
		node->op1 = op2;
		newNode();
		
		node->type = "st";
		node->op1 = "b";
		node->tabCount = tabCount;
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
		node->op1 = "c";
		newNode();
		
		
	} else {
		node->type = optype;
		node->op1 = op2;
		node->tabCount = tabCount;
		newNode();
	}
	
	if (arrOffset == 1) {
			//keep the result in b register
			node->type = "st";
			node->tabCount = tabCount;
			node->op1 = "b";
			newNode();
			
			//load the variable offset
			node->type = "ld";
			node->tabCount = tabCount;
			node->op1 = getOp("$", 1, getAddressString(findID(offsetID)->address));
			newNode();
			
			//store offset in c register
			node->type = "st";
			node->tabCount = tabCount;
			node->op1 = "c";
			newNode();
			
			
			//load the result back into acc
			node->type = "ld";
			node->tabCount = tabCount;
			node->op1 = "b";
			newNode();
			
			//store the result to the offset plus the known address
			node->type = "st";
			node->tabCount = tabCount;
			char addr[4];
			node->op1 = "c";
			sprintf(addr, "%x", destination);
			node->op2 = "+";
			node->op3 = getOp("$", 1, addr);
			newNode();
			
		} else {
	
		node->type = "st";
		node->tabCount = tabCount;
		char dest[4];
		sprintf(dest, "%x", destination);
		node->op1 = getOp("$", 1, dest);
		newNode();
		
		}

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
		node->type = "mov";
		node->tabCount = tabCount;
		node->op1 = strcat(getOp("#", 1, token->lexeme), ",");
		node->op2 = "acc";
		newNode();
	} else {
		expect(ID);
		
		if (IDInUse(token->lexeme) == 0) {
			printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
			exit(-1);
		}
		
		int address = (int) findID(token->lexeme)->address;
		
		
		if (peek(1) == LBRACKET) {
			expect(LBRACKET);
			if (peek(1) == NUM) {
				expect(NUM);
				
				address += atoi(token->lexeme);
				
				node->type = "ld";
				node->tabCount = tabCount;
				node->op1 = getOp("$", 1, getAddressString(address));
				newNode();
				
			} else if (peek(1) == ID) {
				expect(ID);
				
				if (IDInUse(token->lexeme) == 0) {
					printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
					exit(-1);
				}
				
				int offsetAddress = (int) findID(token->lexeme)->address;
				
				//load the value of the offset variable
				node->tabCount = tabCount;
				node->type = "ld";
				node->op1 = getOp("$", 1, getAddressString(offsetAddress));
				newNode();	
				
				//store the offset in c
				node->tabCount = tabCount;
				node->type = "st";
				node->op1 = "c";
				newNode();
				
				//fetch the value being accessed
				node->tabCount = tabCount;
				node->type = "ld";
				node->op1 = "c";
				node->op2 = "+";
				node->op3 = getOp("$", 1, getAddressString(address));
				newNode();
				
			}
			expect(RBRACKET);
			
			} else {
				node->type = "ld";
				node->tabCount = tabCount;
				node->op1 = getOp("$", 1, getAddressString(address));
				newNode();
			}
			
		
	}
	
	

	if (peek(1) == OPC) {
	expect(OPC);

	//store the first operand in the b register	
	node->type = "st";
	node->tabCount = tabCount;
	node->op1 = "b";
	newNode();
	
	char branchType;
	char twoBranches;
	
	if (strcmp(token->lexeme, "==") == 0) {
		branchType = 1;
	} else if (strcmp(token->lexeme, "!=") == 0) {
		branchType = 0;
	}
	
	//TODO implement branches for if OPC is >= <= < or > (do a be/bne, then check CY to see if it's one using bpc or bn 
	
	// > and < are easy enough; if they're equal, then the condition is automatically false. If not, then we can use CY to check which operand is greater than or less than
	
	// >= and <= can be interpreted as the opposites of < and >, respectively. Therefore, a fail on a < or a > means a success for a >= or a <=
	else if (strcmp(token->lexeme, "<") == 0) {
		branchType = 0;
		twoBranches = 1;
	} else if (strcmp(token->lexeme, ">") == 0) {
		branchType = 0;
		twoBranches = 2;
	} else if (strcmp(token->lexeme, "<=") == 0) {
		branchType = 1;
		twoBranches = 3;
	} else if (strcmp(token->lexeme, ">=") == 0) {
		branchType = 1;
		twoBranches = 4;
	}
		

	if (peek(1) == NUM) {
		expect(NUM);
		
		node->type = "mov";
		node->tabCount = tabCount;
		node->op1 = strcat(getOp("#", 1, token->lexeme), ",");
		node->op2 = "acc";
		newNode();
		
		
	} else {
		expect(ID);
		
		if (IDInUse(token->lexeme) == 0) {
			printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
			exit(-1);
		}
		
		int address = (int) findID(token->lexeme)->address;
		
		
		if (peek(1) == LBRACKET) {
			expect(LBRACKET);
			if (peek(1) == NUM) {
				expect(NUM);
				
				address += atoi(token->lexeme);
				
				node->type = "ld";
				node->tabCount = tabCount;
				node->op1 = getOp("$", 1, getAddressString(address));
				newNode();
				
			} else if (peek(1) == ID) {
				expect(ID);
				
				if (IDInUse(token->lexeme) == 0) {
					printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
					exit(-1);
				}
				
				int offsetAddress = (int) findID(token->lexeme)->address;
				
				//load the value of the offset variable
				node->tabCount = tabCount;
				node->type = "ld";
				node->op1 = getOp("$", 1, getAddressString(offsetAddress));
				newNode();	
				
				//store the offset in c
				node->tabCount = tabCount;
				node->type = "st";
				node->op1 = "c";
				newNode();
				
				//fetch the value being accessed
				node->tabCount = tabCount;
				node->type = "ld";
				node->op1 = "c";
				node->op2 = "+";
				node->op3 = getOp("$", 1, getAddressString(address));
				newNode();
				
			}
			expect(RBRACKET);
			
			} else {
				node->type = "ld";
				node->tabCount = tabCount;
				node->op1 = getOp("$", 1, getAddressString(address));
				newNode();
			}
			
	}
	
	//for == and !=
	if (twoBranches == 0) {
		if (branchType == 0) {
			node->type = "be";
		} else {
			node->type = "bne";
		}
		node->tabCount = tabCount;
		node->op1 = "b,";
	} else {
		//TODO write instructions for < > >= and <=
		char* subBranchColon = (char*) malloc(20);
		char* subBranch = (char*) malloc(19);
		if (ifOrWhile == 0) {
			sprintf(subBranchColon, ".if%x:", ifNumber-1);
			sprintf(subBranch, ".if%x", ifNumber-1);
		} else {
			sprintf(subBranchColon, ".while%x:", whileNumber-1);
			sprintf(subBranch, ".while%x", whileNumber-1);
		}
		
		node->type = "clr1";
		node->tabCount = tabCount;
		node->op1 = "acc,";
		node->op2 = "cy";
		newNode();
		
		if (twoBranches == 1) { // op1 < op2
			//check for equality (automatic fail)
			node->type = "be";
			node->tabCount = tabCount;
			node->op1 = "b,";
			node->op2 = subBranch;
			newNode();
			
			//check if op2 < op1 (also a fail)
			node->type = "bpc";
			node->tabCount = tabCount;
			node->op1 = "acc,";
			node->op2 = "cy,";
			
			
		} else if (twoBranches == 2) { // op1 > op2
			//swap positions of operands so that op2 is in acc, then do the same as a greater than operation
			node->type = "st";
			node->tabCount = tabCount;
			node->op1 = "c";
			newNode(); 
			
			node->type = "ld";
			node->tabCount = tabCount;
			node->op1 = "b";
			newNode();
			
			//check for equality op1 = op2 (automatic fail)
			node->type = "be";
			node->tabCount = tabCount;
			node->op1 = "c,";
			node->op2 = subBranch;
			newNode();
			
			//check if op1 < op2 (also a failure)
			node->type = "bpc";
			node->tabCount = tabCount;
			node->op1 = "acc,";
			node->op2 = "cy,";
			

		} else if (twoBranches == 3) { // <=
			
			//create a new label (points to inside the if block, taken on success)
			char* successColon = (char*) malloc(20);
			char* success = (char*) malloc(19);
			if (ifOrWhile == 0) {
				sprintf(successColon, ".ifs%x:", ifNumber-1);
				sprintf(success, ".ifs%x", ifNumber-1);
			} else {
				sprintf(success, ".whiles%x:", whileNumber-1);
				sprintf(successColon, ".whiles%x", whileNumber-1);
			}
			
			//Check for equality (automatic success)
			node->type = "be";
			node->tabCount = tabCount;
			node->op1 = "b,";
			node->op2 = success;
			newNode();
			
			//Check if less than bit is on (also success)
			node->type = "bpc";
			node->tabCount = tabCount;
			node->op1 = "acc,";
			node->op2 = "cy,";
			node->op3 = success;
			newNode();
			
			//branch to failure (only reach here if checks fail)
			node->type = "br";
			node->tabCount = tabCount;
			node->op1 = subBranch;
			newNode();
			
			//Label the success
			node->header = successColon; 
			
		} else if (twoBranches == 4) { // >=
			
			//create a new label (points to inside the if block, taken on success)
			char* successColon = (char*) malloc(20);
			char* success = (char*) malloc(19);
			if (ifOrWhile == 0) {
				sprintf(successColon, ".ifs%x:", ifNumber-1);
				sprintf(success, ".ifs%x", ifNumber-1);
			} else {
				sprintf(success, ".whiles%x:", whileNumber-1);
				sprintf(successColon, ".whiles%x", whileNumber-1);
			}
			
			//Check for equality (automatic success)
			node->type = "be";
			node->tabCount = tabCount;
			node->op1 = "b,";
			node->op2 = success;
			newNode();
			
			//Check if less than bit is on (failure)
			node->type = "bpc";
			node->tabCount = tabCount;
			node->op1 = "acc,";
			node->op2 = "cy,";
			node->op3 = subBranch;
			newNode();
			
			//Label the success
			node->header = successColon; 
		}
	}
	
	
	
	//No comparison means checking if the value is not zero; jump if that check fails
	} else {
		node->type = "bz";
		node->tabCount = tabCount;
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
	
	ifOrWhile = 0;
	char* pointColon = (char*) malloc(20);
	char* point = (char*) malloc(19);
	sprintf(pointColon, ".if%x:", ifNumber);
	sprintf(point, ".if%x", ifNumber);
	ifNumber++;
	
	expect(LPAREN);

	parseCondition();
	
	//appended to the branch instruction
	
	if (node->header == NULL) {
		node->op3 = point;
		newNode();
	}

	expect(RPAREN);

	expect(LBRACE);
	IDName* prevIDNames = IDNames;

	parseInstructionList();
	IDNames = prevIDNames;

	//label points to right after the if statement
	node->header = pointColon;

	expect(RBRACE);
}

void parseWhile() {
	expect(WHILE);
	
	ifOrWhile = 1;
	char* pointColon = (char*) malloc(20);
	char* point = (char*) malloc(19);
	sprintf(pointColon, ".while%x:", whileNumber);
	sprintf(point, ".while%x", whileNumber);
	
	char* startColon = (char*) malloc(20);
	char* start = (char*) malloc(19);
	sprintf(startColon, ".start%x:", whileNumber);
	sprintf(start, ".start%x", whileNumber);
	
	whileNumber++;
	
	expect(LPAREN);

	node->header = startColon;

	parseCondition();
	
	//appended to the branch instruction
	
	if (node->header == NULL) {
		node->op3 = point;
		newNode();
	}
	
	expect(RPAREN);
	expect(LBRACE);
	IDName* prevIDNames = IDNames;
	
	parseInstructionList();
	IDNames = prevIDNames;
	
	node->type = "br";
	node->tabCount = tabCount;
	node->op1 = start;
	newNode();
	
	//label points to right after the while statement (taken on condition failure)
	node->header = pointColon;
	
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
		
		
	} else if (peek(1) == ID) {
		expect(ID);
		
		if (IDInUse(token->lexeme) == 0) {
			printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
			exit(-1);
		}
		
		int address = (int) findID(token->lexeme)->address;
		
		
		if (peek(1) == LBRACKET) {
			expect(LBRACKET);
			if (peek(1) == NUM) {
				expect(NUM);
				
				address += atoi(token->lexeme);
				
				node->type = "ld";
				node->tabCount = tabCount;
				node->op1 = getOp("$", 1, getAddressString(address));
				newNode();
				
			} else if (peek(1) == ID) {
				expect(ID);
				
				if (IDInUse(token->lexeme) == 0) {
					printf("ERROR: variable/function \"%s\" is referenced but never declared\n", token->lexeme);
					exit(-1);
				}
				
				int offsetAddress = (int) findID(token->lexeme)->address;
				
				//load the value of the offset variable
				node->tabCount = tabCount;
				node->type = "ld";
				node->op1 = getOp("$", 1, getAddressString(offsetAddress));
				newNode();	
				
				//store the offset in c
				node->tabCount = tabCount;
				node->type = "st";
				node->op1 = "c";
				newNode();
				
				//fetch the value being accessed
				node->tabCount = tabCount;
				node->type = "ld";
				node->op1 = "c";
				node->op2 = "+";
				node->op3 = getOp("$", 1, getAddressString(address));
				newNode();
				
			}
			expect(RBRACKET);
			
			} else {
				node->type = "ld";
				node->tabCount = tabCount;
				node->op1 = getOp("$", 1, getAddressString(address));
				newNode();
			}
			
	}
	
	node->type = "push";
	node->tabCount = tabCount;
	node->op1 = "acc";
	newNode();
	
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
	ifNumber = 0;
	whileNumber = 0;
	
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
