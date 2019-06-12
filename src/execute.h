#ifndef EXECUTE_H
#define EXECUTE_H

#include "emulate_utils.h"

void execute_processing(uint32_t *registers, Decoded_Instruction *decoded_instr);

void execute_multiplying(uint32_t *registers, Decoded_Instruction const *decoded_instr);

void execute_transferring(uint32_t *registers, uint8_t *memory, uint8_t *gpio, uint8_t *gpio_on,
                          uint8_t *gpio_off, Decoded_Instruction *decoded_instr);

void execute_branch(uint32_t *pc_register, Decoded_Instruction const *decoded_instr);

#endif
