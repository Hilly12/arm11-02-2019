#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "symbolTable.h"
#include "ioManager.h"

#define MAX_LINE_LENGTH 511

typedef struct instruction {
    char *mnemonic;
    uint8_t Rn;
    uint8_t Rd;
    uint8_t Rs;
    uint8_t Rm;
    uint8_t Opcode;
    uint8_t A;
    uint8_t S;
    uint8_t I;
    uint8_t P;
    uint8_t U;
    uint8_t L;
    uint16_t Op2;
} instruction;

uint32_t processInstruction(char *code, SymbolTable* symbolTable) {
    uint32_t binaryInstruction = 0;


    //TODO: Get mnemonic from the start of the instruction


    //TODO: Work out which instruction it is and format (Big switch case?)

    //TODO: Create binary encoding based on instruction and format
    //Formats have common elements so use functions to set those specific bits
    //For the branch instructions have to use the symbol table to get address, lookup function needed;
    //Optional: Function needed to work out operand 2 for shifted register case

    return binaryInstruction;
}

// Uses array of pointers to point to 
char **init2dCharArray(int rows, int cols) {
    char **res = (char **) malloc(rows * sizeof(char *));
    res[0] = (char *) malloc(rows * cols * sizeof(char));
    for (int i = 1; i < rows; i++) {
        res[i] = res[i - 1] + cols;
    }
    return res;
}

int main(int argc, char **argv) {
    // Load file into array
    // FILE *fileIn;
    // fileIn = fopen(argv[1], "r");
    // fclose(fileIn);
    // const int numLines = getNumberOfLines(fileIn);

    int numLines = 0;
    char *data;

    loadFile(argv, MAX_LINE_LENGTH, &numLines, data);

    char **instructionsStrArray = init2dCharArray(numLines, MAX_LINE_LENGTH);
    fileToArrayLineByLine(numLines, MAX_LINE_LENGTH, data, instructionsStrArray);

    // Generate symbol table (Pass 1)
    SymbolTable *symbolTable = createTable();
    char *label = (char *) malloc(MAX_LINE_LENGTH * sizeof(char));
    char label[MAX_LINE_LENGTH];
    int address = 0;
    // memset(label, 0, sizeof(label));
    for (int i = 0; i < numLines; i++) {
        // // For each instruction remove and new line characters
        // for (int j = 0; j < MAX_LINE_LENGTH; j++) {
        //     if (instructionsStr[i][j] == '\n') {
        //         instructionsStr[i][j] = '\0';
        //     }
        // }
        // Reset the label buffer
        // memset(label, 0, sizeof label);
        // // Saves the address of a line if its in the format of a label
        // for (int j = 0; j < MAX_LINE_LENGTH; j++) {
        //     if (instructionsStr[i][j] == ':') {
        //         label[j] = '\0';
        //         addEntry(s, label, i * 4);
        //         break;
        //     } else if (instructionsStr[i][j] == ' ') {
        //         break;
        //     } else {
        //         label[j] = instructionsStr[i][j];
        //     }
        // }
        if (strstr(instructionsStrArray, ":") != NULL) { // If ':' is in the line
            label = strdup(instructionsStrArray[i]);
            label[strlen(label) - 1] = '\0';
            addEntry(symbolTable, label, address * 4);
        } else {
            address++;
        }
    }


    //TODO: Generate binary encoding for each line (Pass 2)
    //Pre: Array of instructions and a adt holding a symbol table
    //Post: An array of binary instructions

    uint32_t * instructions;

    for (int i = 0; i < numLines; i++) {
        if (strstr(instructionsStrArray, ":") == NULL) {
            instructions[i] = processInstruction(instructionsStrArray[i], symbolTable);
        }
    }

    // Save file
    saveToFile(argv[2], instructions);

    return EXIT_SUCCESS;
}
