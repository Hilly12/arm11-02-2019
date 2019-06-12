#ifndef PIPELINE_H
#define PIPELINE_H

#include "decode.h"
#include "execute.h"

// Fetches instruction from memory at address pointed to by pc_val and stores it
// in instr[8] in 4 bit blocks
void fetch(uint32_t const *pc_val, uint8_t const *memory, uint8_t *instr);

// Decodes the instruction instr from its 4 bit block form to a form which can be simply executed
void decode(uint8_t const *instr, uint32_t const *registers, Decoded_Instruction *decoded_instr);

// Executes the decoded instruction and modifies registers and memory
void execute(uint32_t *registers, uint8_t *memory, uint8_t *gpio, uint8_t *gpio_on,
             uint8_t *gpio_off, Decoded_Instruction *decoded_instr);

#endif
