#include <stdlib.h>
#include <stdio.h>
#include <string.h>
typedef unsigned char BYTE;

//Function that converts an array of instructions to an array of bytes
//in the style that appears to how it does in memory
BYTE * instructionsToMemory(BYTE *memory, int *instructions) {
    size_t n = sizeof(instructions) / sizeof(int);
    int instrBuffer;
    for (int i = 0; i < n; ++i) {
        instrBuffer = instructions[i];

        memory[4*i]     = (instrBuffer & 0xFF);
        memory[4*i + 1] = ((instrBuffer >> 8) & 0xFF);
        memory[4*i + 2] = ((instrBuffer >> 16) & 0xFF);
        memory[4*i + 3] = ((instrBuffer >> 24) & 0xFF);
    }

    return memory;
}



int main(int argc, char **argv) {
    //TODO: Load file in
    FILE *fileIn;
    fileIn = fopen(argv[1], "r");

    //TODO: Create array where each element is a different instruction

    //TODO: Generate symbol table

    //TODO: Generate binary encoding for each line


    //Given an array of instructions, saves to a binary file how it would be represented in memory
    FILE *fileOut;
    fileOut = fopen(argv[2], "w+");
    BYTE memory[sizeof(instructions)];
    BYTE *output = instructionsToMemory(memory, instructions);

    fwrite(output, sizeof(memory), 1, fileOut);
    fclose(fileOut);


    return EXIT_SUCCESS;
}
