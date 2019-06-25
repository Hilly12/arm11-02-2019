#ifndef PIPELINE_H
#define PIPELINE_H

#include "execute.h"

// Fetches instruction from memory at address pointed to by pc_val and stores it in the array instr[8] in 4 bit blocks
void fetch(unsigned int const *pc_val, byte const *memory, byte *instr);

// Decodes the instruction instr from its 4 bit block form to a form which can be simply executed
void decode(byte const *instr, unsigned int const *registers, Decoded_Instruction *decoded_instr);

// Executes the decoded instruction and modifies registers and memory
void execute(unsigned int *registers, byte *memory, byte *gpio, byte *gpio_on,
             byte *gpio_off, Decoded_Instruction *decoded_instr);

#endif
