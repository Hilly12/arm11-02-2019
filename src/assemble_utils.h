#ifndef ASSEMBLEUTILS_H
#define ASSEMBLEUTILS_H

#include <stdio.h>
#include "symbol_table.h"

// Returns a 32 bit mov instruction depending on the input. Used for ldr and lsl instructions that are represented by
// the binary code of a mov instruction
int special_to_move_instruction(byte Rd, byte I, byte operand, byte shift_operand);

// Writes 32 bit data into 8 bit memory block by block starting at given address accounting for endianness
void write_4byte_to_memory(byte *memory, unsigned int const *data, int address);

// Saves contents of memory to a binary file named file_name last_instr is the size
void save_to_file(char const *file_name, byte const *memory, unsigned int const *last_instr);

#endif
