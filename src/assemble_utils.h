#ifndef ASSEMBLEUTILS_H
#define ASSEMBLEUTILS_H

#include <stdint.h>
#include <stdio.h>
#include "symbol_table.h"

// Returns a 32 bit mov instruction depending on the input
int to_move_instruction(uint8_t Rd, uint8_t I, uint8_t operand, uint8_t shift_type, uint8_t shift_operand);

// Generate offset for branch instruction
int generate_branch_offset(Symbol_Table *sym_table, char *label, int cuur_instr);

// Writes 32 bit data into 8 bit memory block by block starting at given address
// accounting for endianness
void write_4byte_to_memory(uint8_t *memory, int data, int address);

// Saves contents of memory to a binary file named file_name
// last_instr is the size
void save_to_file(char *file_name, uint8_t *memory, int last_instr);

#endif
