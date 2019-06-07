#include <stdint.h>
#include <stdio.h>

// Uses array of pointers to point to 
char **init2dCharArray(int rows, int cols);

// Loads the file into a string and counts the number of lines
// of assembly code in the file
char **loadFile(char **argv, int maxLineLength, int *numLines);

// Function that converts an array of instructions to an array of bytes
// in the style that appears to how it does in memory
void instructionsToMemory(uint8_t *memory, int const *instructions, int instructionCount);

// Given an array of binary instructions, saves to a binary file how it would be represented in memory
void saveToFile(char *filename, int const *instructions, int instructionCount);
