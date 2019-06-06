#include <stdint.h>
#include <stdio.h>

typedef char BYTE;

// Loads the file into a string and counts the number of lines
// of assembly code in the file
char * loadFile(FILE *file, int maxLineLength, int *numLines);

// Loads data into a String array containing each line of assembly code
void fileToArrayLineByLine(int numLines, char *data, char **lines);

// Function that converts an array of instructions to an array of bytes
// in the style that appears to how it does in memory
BYTE *instructionsToMemory(BYTE *memory, int *instructions);

// Given an array of binary instructions, saves to a binary file how it would be represented in memory
void saveToFile(char *filename, int *instructions);