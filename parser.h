/*
 * parser.h
 *
 *  Created on: May 11, 2025
 *      Author: lordbananza
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "lexer.h"

typedef enum Type {
	//These are all instructions supported by the VMU's architecture
	ADD, ADDC, SUB, SUBC, INC, DEC, MUL, DIV, AND, OR, XOR, ROL, ROLC, ROR, RORC,
	LD, ST, MOV, LDC, PUSH, POP, XCH, JMP, JMPF, BR, BRF, BZ, BNZ, BP, BPC, BN, DBNZ,
	BE, BNE, CALL, CALLF, CALLR, RET, RETI, CLR1, SET1, NOT1, NOP
}Type;

//Basic
typedef struct Instruction {

	Type type;

	int op1;

	int op2;

	int destination;

	struct Instruction* target;

	struct Instruction* next;

}Instruction;

Instruction* parseTokens(Token* head);



#endif /* PARSER_H_ */
