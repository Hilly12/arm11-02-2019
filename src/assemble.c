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

int commandToInstruction(char *instruction) { //should also include the symbol table
    //TODO: Get mnemonic from the start of the instruction

    //TODO: Work out which instruction it is and format (Big switch case?)

    //TODO: Create binary encoding based on instruction and format
    //Formats have common elements so use functions to set those specific bits
    //For the branch instructions have to use the symbol table to get address, lookup function needed;
    //Optional: Function needed to work out operand 2 for shifted register case

    int binaryInstruction = 0;


    return binaryInstruction;
}





int main(int argc, char **argv) {
    //TODO: Load file in
    //Pre: A file holding instructions
    //Post: File stored in memory
    FILE *fileIn;
    fileIn = fopen(argv[1], "r");
    //Read in
    fclose(fileIn);


    //TODO: Create array where each element is a different instruction
    //Pre: File is in memory
    //Post: An array of instructions in string format split at \n or \0

    char instructionsStr[][];

    //TODO: Generate symbol table (Pass 1)
    //Pre: An array of instructions in string format
    //Post: An ADT holding a symbol table

    //Idea for symbol table is to create a linked list of label and address pairs

    //TODO: Generate binary encoding for each line (Pass 2)
    //Pre: Array of instructions and a adt holding a symbol table
    //Post: An array of binary instructions

    int instructions[sizeof(instructionsStr) / sizeof(instructionsStr[0]]


    for( i = 0; i < sizeof(instructionsStr) / sizeof(instructionsStr[0]); i++)
    {
        instructions[i] = commandToInstruction(instructionsStr[i]);
    }

    //Given an array of binary instructions, saves to a binary file how it would be represented in memory
    FILE *fileOut;
    fileOut = fopen(argv[2], "w+");
    BYTE memory[sizeof(instructions)];
    BYTE *output = instructionsToMemory(memory, instructions);

    fwrite(output, sizeof(memory), 1, fileOut);
    fclose(fileOut);


    return EXIT_SUCCESS;
}
