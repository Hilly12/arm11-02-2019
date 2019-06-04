#include <stdint.h>
#include <stdio.h>

// Loads the file into a string and counts the number of lines
// of assembly code in the file
void loadFile(char **argv, int maxLineLength, int *numLines, char* data);

// Loads data into a String array containing each line of assembly code
void fileToArrayLineByLine(int numLines, char *data, char **lines);

// Function that converts an array of instructions to an array of bytes
// in the style that appears to how it does in memory
uint8_t *instructionsToMemory(uint8_t *memory, uint32_t *instructions);

// Given an array of binary instructions, saves to a binary file how it would be represented in memory
void saveToFile(char *filename, int *instructions);