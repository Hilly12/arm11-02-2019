#ifndef EXECUTE_H
#define EXECUTE_H

#include "decode.h"

void execute_processing(unsigned int *registers, Decoded_Instruction *decoded_instr);

void execute_multiplying(unsigned int *registers, Decoded_Instruction const *decoded_instr);

void execute_transferring(unsigned int *registers, byte *memory, byte *gpio, byte *gpio_on,
                          byte *gpio_off, Decoded_Instruction *decoded_instr);

void execute_branch(unsigned int *pc_register, Decoded_Instruction const *decoded_instr);

#endif
