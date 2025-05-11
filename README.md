# Chao
Language for Writing VMU Programs

**This project is in extremely early developement.** More details and documentation will arrive as the project continues developement.

# Description
The VMU is a programmable memory card, created an accessory to Sega's Dreamcast system. A dedicated community has developed their own tools for writing custom code for it, including the <code>aslc86k</code> assembler by Marcus Comstedt. This program generates assembly code compatible with use alongside aslc86k, using a more C-like syntax. 

# Data Types
Chao supports integer variables of 8, 9, and 16 bits. Their names are, respectively:

<code>int8</code>

<code>int9</code>

<code>int16</code>

Ints are all unsigned.

# Context-Free Grammar (CFG)
The CFG will update as the project continues. The grammar of the Chao is "C-like" in that it adheres to many of C's conventions, but note that the hardware limitations of the VMU prevent many data types and functionsdifficult, if not impossible to implement.
