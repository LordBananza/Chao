# Chao
Language for Writing VMU Programs

**This project is in extremely early developement.** More details and documentation will arrive as the project continues developement.

# Description
Released in 1998, the Sega Dreamcast featured an accessory called the VMU, a memory card with the capability to download small interactive games from the console. Users could play these games independently of the console so long as the VMU was powered by a battery. Like many game consoles, the Dreamcast gathered a dedicated base of people willing to create custom software for it, and the same applies to the VMU, although to a lesser degree. This is in part due to the painstaking process of writing code for it. The VMU’s CPU uses a custom set of instructions, and configuring its additional hardware has more in common with embedded systems than software engineering. At present, the highest level programming language that exists for the VMU is a written form of assembly, which still requires knowledge of the device’s inner workings to truly be capable of writing code. A user-friendly, higher level language would have the potential to alleviate many of the issues which surround VMU programming. With syntax that aligns more closely with C than assembly, programmers would be able to write code without needing to research so heavily on what happens from a hardware perspective, allowing for abstraction between each level of functionality to be introduced to the VMU. This is what Chao intends to be for developers.

This program generates assembly code compatible with use alongside the <code>aslc86k</code> assembler by Marcus Comstedt. In the fututr, it will include libraries for more complex tasks, such as writing output to tbe display, obtaining input from the buttons.

# Data Types
So far, Chao only supports the data type char, which is a 1-byte (or 8-bit) variable. Arrays of type char are also supported.

# Context-Free Grammar (CFG)
The CFG will update as the project continues. The grammar of the Chao is "C-like" in that it adheres to many of C's conventions, but note that the hardware limitations of the VMU prevent many data types and functions difficult, if not impossible to implement.

Code → Function_List

Function_List → Function | Declaration | (Function | Declaration) Function_List

Function → TYPE ID LPAREN Argument_List RPAREN LBRACE Instruction_List RBRACE

Argument_List → null | Argument | Argument COMMA Argument_List

Argument → TYPE ID

Instruction_List → Instruction | Instruction Instruction_List

Instruction → Assignment | Declaration | If | While | Call

Assignment→ID EQUAL (ID | NUM) SEMICOLON | ID EQUAL (ID | NUM) OPERATOR (ID | NUM) SEMICOLON

Declaration → TYPE ID SEMICOLON | TYPE ID EQUAL NUM SEMICOLON

If → IF LPAREN Condition RPAREN LBRACE Instruction_List RBRACE

Condition →Comp_List

Comp_List → Comp | Comp (AND/OR) Comp_List

Comp → (NUM|ID) OPC (NUM|ID)

While → WHILE LPAREN Condition RPAREN LBRACE Instruction_List RBRACE

Call → ID LPAREN Parameter_List RPAREN SEMICOLON

Parameter_List → Parameter | Parameter COMMA Parameter_List

Parameter → ID


ID → (a*b*c*d*e*f*g*h*i*j*k*l*m*n*m*o*p*q*r*s*t*u*v*w*x*y*z*)* | (a*b*c*d*e*f*g*h*i*j*k*l*m*n*m*o*p*q*r*s*t*u*v*w*x*y*z*)*(LBRACKET NUM|ID RBRACKET)

NUM → (0*1*2*3*4*5*6*7*8*9*)*

EQUAL → =

SEMICOLON → ;

LPAREN → (

RPAREN → )

LBRACE → {

RBRACE → }

LBRACKET = [

RBRACKET = ]

COMMA → ,

AND/OR → && | ||

OPERATOR → + | - | * | / | << | >> | & | “|” | ^ | ~

OPC → > | < | >= | <= | == | !=

TYPE → void | char
