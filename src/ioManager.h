#include <stdint.h>
#include <stdio.h>

typedef char BYTE;

// Uses array of pointers to point to 
char **init2dCharArray(unsigned int rows, unsigned int cols);

// Loads the file into a string and counts the number of lines
// of assembly code in the file
char **loadFile(char **argv, int maxLineLength, int *numLines);

// Function that converts an array of instructions to an array of bytes
// in the style that appears to how it does in memory
BYTE *instructionsToMemory(BYTE *memory, int *instructions);

// Given an array of binary instructions, saves to a binary file how it would be represented in memory
void saveToFile(char *filename, int *instructions);