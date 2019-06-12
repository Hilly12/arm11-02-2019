#ifndef DECODE_H
#define DECODE_H

#include "emulate_utils.h"

void decode_processing(uint8_t const *instr, uint32_t const *registers, Decoded_Instruction *decoded_instr);

void decode_multiplying(uint8_t const *instr, Decoded_Instruction *decoded_instr);

void decode_transferring(uint8_t const *instr, uint32_t const *registers, Decoded_Instruction *decoded_instr);

void decode_branch(uint8_t const *instr, Decoded_Instruction *decoded_instr);

#endif
