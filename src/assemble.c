#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "symbolTable.h"

#define LINELENGTH 511

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

//Given an array of binary instructions, saves to a binary file how it would be represented in memory
void saveToFile(char * filename, int *instructions) {
    FILE *fileOut;
    fileOut = fopen(filename, "w+");
    BYTE memory[sizeof(instructions)];
    BYTE *output = instructionsToMemory(memory, instructions);
    fwrite(output, sizeof(memory), 1, fileOut);
    fclose(fileOut);
}

int getNumberOfLines(FILE *file) {
    int lines = 0;
    int ch = 0;
    while(!feof(file))
    {
        ch = fgetc(file);
        if((ch == '\n' ) | (ch == EOF))
        {
            lines++;
        }
    }
    rewind(file);
    return lines;
}

void fileToArrayLineByLine(int noLines, int lineLength, FILE *file, char lines[][lineLength]) {
    for (int i = 0; i < noLines; i++) {
        fgets(lines[i], lineLength, file);
    }
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
    //Load file into array
    FILE *fileIn;
    fileIn = fopen(argv[1], "r");
    int lines = getNumberOfLines(fileIn);
    char instructionsStr[lines][LINELENGTH];
    fileToArrayLineByLine(lines, LINELENGTH, fileIn, instructionsStr);
    fclose(fileIn);

    //Generate symbol table (Pass 1)
    SymbolTable * s = createTable();
    char label[LINELENGTH];
    memset(label, 0, sizeof label);
    for(int i = 0; i < lines; i++)
    {
        //For each instruction remove and new line characters
        for(int j = 0; j < LINELENGTH; j++)
        {
            if(instructionsStr[i][j]=='\n')
            {
                instructionsStr[i][j]='\0';
            }

        }
        //Reset the label buffer
        memset(label, 0, sizeof label);

        //Saves the address of a line if its in the format of a label
        for (int j = 0; j < LINELENGTH; j++) {
            if (instructionsStr[i][j] == ':') {
                label[j] = '\0';
                addEntry(s, label, i * 4);
                break;
            } else if (instructionsStr[i][j] == ' ') {
                break;
            } else {
                label[j] = instructionsStr[i][j];
            }
        }
    }


    //TODO: Generate binary encoding for each line (Pass 2)
    //Pre: Array of instructions and a adt holding a symbol table
    //Post: An array of binary instructions

    int instructions[lines];

    for(int i = 0; i < lines; i++)
    {
        instructions[i] = commandToInstruction(instructionsStr[i]);
    }

    //Save file
    saveToFile(argv[2], instructions);

    return EXIT_SUCCESS;
}
