#ifndef ASSEMBLEUTILS_H
#define ASSEMBLEUTILS_H

#include <stdint.h>
#include <stdio.h>
#include "symbol_table.h"

// Writes 32 bit data into 8 bit memory block by block starting at given address
// accounting for endianness
void write4ByteToMemory(uint8_t *memory, int data, int address);

// Saves contents of memory to a binary file named filename
// lastInstr is the size 
void saveToFile(char *filename, uint8_t *memory, int lastInstr);

// Generate offset for branch instruction
int generateBranchOffset(SymbolTable * symbolTable, char *label, int currInstr);

// Returns a 32 bit mov instruction depending on the input
int toMoveInstruction(uint8_t Rd, uint8_t I, uint8_t Operand, uint8_t shiftType, uint8_t shiftOperand);

#endif