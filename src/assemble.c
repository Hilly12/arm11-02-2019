#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "symbolTable.h"
#include "ioManager.h"

#define MAX_LINE_LENGTH 511

typedef struct instruction {
    char *mnemonic;
    uint8_t Cond;
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
    uint32_t lastAddress;
} Instruction;

uint8_t parseRegister(char *token) {
    uint8_t r = token[1] - '0';
    if (token[2] == '\0') {
        return r;
    }
    return r * 10 + (token[2] - '0');
}

uint16_t parseOperand(char *token) {
    char *op = strtok_r(token, "r#=", NULL);
    if (strstr(token, "x") != NULL) { // Hex
        return strtol(op, NULL, 16);
    } else { // Number or Register
        return strtol(op, NULL, 10);
    }
}

int parseDataProcessing(char* code, char* save, Instruction *instr, SymbolTable *st) {
    instr->Cond = 0xe;
    
    // TODO: add mnemonic to symbol table at start of program
    instr->Opcode = getAddress(st, instr->mnemonic);

    char *token;
    
    if (0x8 <= instr->Opcode && instr->Opcode <= 0x10) { // tst, teq, cmp
        instr->Rn = 0;
        instr->S = 1;
    } else {
        token = strtok_r(code, ", ", &save);
        instr->Rn = parseRegister(token);
        instr->S = 0;
    }

    if (instr->Opcode == 0xd) { // mov
        instr->Rd = 0;
    } else {
        token = strtok_r(code, ", ", &save);
        instr->Rd = parseRegister(token);
    }

    token = strtok_r(code, ", ", &save);
    instr->I = (token[0] == '#');
    instr->Op2 = parseOperand(token);


    token = strtok_r(code, ", ", &save);
    if (token != NULL) { // Shift
        char *shiftToken, *shiftSave;
        uint8_t shiftType;
        uint16_t shiftAmount;

        shiftToken = strtok_r(token, " ", &shiftSave);
        shiftType = getAddress(st, shiftToken) & 0xff;

        shiftToken = strtok_r(token, " ", &shiftSave);
        shiftAmount = parseOperand(shiftToken);
        
        switch(shiftType) {
            case 0: // lsl
                instr->Op2 = instr->Op2 << shiftAmount;
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
        }
    }

    return (instr->Cond << 28) | (instr->I << 25) 
            | (instr->Opcode << 21) | (instr->S << 20)
            | (instr->Rn << 16) | (instr->Rd << 12)
            | instr->Op2;
}

int parseMultiply(char* code, char* save, Instruction *instr, SymbolTable *st) {
    instr->Cond = 0xe;

    instr->S = 0;

    char *token;
    
    token = strtok_r(code, ", ", &save);
    instr->Rd = parseRegister(token);

    token = strtok_r(code, ", ", &save);
    instr->Rm = parseRegister(token);

    token = strtok_r(code, ", ", &save);
    instr->Rs = parseRegister(token);


    token = strtok_r(code, ", ", &save);
    if (token != NULL) { // mla
        instr->Rn = parseRegister(token);
        instr->A = 1;
    }

    return (instr->Cond << 28) | (instr->A << 21) 
            | (instr->S << 20) | (instr->Rd << 16)
            | (instr->Rn << 12) | (instr->Rs << 8)
            | (0x9 << 4) | instr->Rm;
}

int parseDataTransfer(char* code, char* save, Instruction *instr, SymbolTable *st) {
    instr->Cond = 0xe;

    char *token;
    
    token = strtok_r(code, ", ", &save);
    instr->Rn = parseRegister(token);

    // This one is gonna be cancerous
    token = strtok_r(code, ", ", &save);
    instr->Rd = parseRegister(token); // haha if only :')

    return (instr->Cond << 28) | (1 << 26) 
            | (instr->I << 25) | (instr->P << 24)
            | (instr->U << 23) | (instr->L << 20)
            | (instr->Rn << 16) | (instr->Rd << 12)
            | instr->Op2;
}

int parseBranch(char* code, char* save, Instruction *instr, SymbolTable *st) {
    char *token;
    uint32_t offset;
    
    token = strtok_r(code, " b", &save);
    instr->Cond = getAddress(st, token);

    token = strtok_r(code, " ", &save);
    offset = generateOffset(st, token, 0x0); // TODO: pass address

    return (instr->Cond << 28) | (0xa << 24) | offset;
}

int processInstruction(char *code, SymbolTable *symbolTable) {
    Instruction *instr = malloc(sizeof(Instruction));
    char *save;

    instr->mnemonic = strtok_r(code, " ", &save);

    if (strcmp(instr->mnemonic, "andeq") == 0) {
        return 0;
    }

    // void (*parse[]) (char*, char*, Instruction*, SymbolTable*) = { parseDataProc };


    //TODO: Get mnemonic from the start of the instruction

    //TODO: Work out which instruction it is and format (Big switch case?)

    //TODO: Create binary encoding based on instruction and format
    //Formats have common elements so use functions to set those specific bits
    //For the branch instructions have to use the symbol table to get address, lookup function needed;
    //Optional: Function needed to work out operand 2 for shifted register case

    return parseDataProc(code, save, instr, symbolTable);
}

// Uses array of pointers to point to 
char **init2dCharArray(unsigned int rows,unsigned int cols) {
    char **res = (char **) malloc(rows * sizeof(char *));
    res[0] = (char *) malloc(rows * cols * sizeof(char));
    for (int i = 1; i < rows; i++) {
        res[i] = res[i - 1] + cols;
    }
    return res;
}

//Free space allocated to a 2d array
void free2dArray(char **array, unsigned int rows) {
    for (int i = 0; i < rows; i++) {
        free(array[i]);
    }
    free(array);
}

// Generate offset for branch instruction
int generateOffset(SymbolTable * symbolTable, char *label, int currentAddress) {
    return (getAddress(symbolTable, label) - currentAddress - 4) >> 2;
}

int main(int argc, char **argv) {
    //Load file in
    int numLines = 0;
    char *data;
    FILE *fileIn;
    fileIn = fopen(argv[1], "r");
    data = loadFile(fileIn, MAX_LINE_LENGTH, &numLines);
    fclose(fileIn);

    //Convert data into array that can be read from
    char **instructionsStrArray = init2dCharArray(numLines, MAX_LINE_LENGTH);
    fileToArrayLineByLine(numLines, data, instructionsStrArray);

    // Generate symbol table (Pass 1)
    SymbolTable *symbolTable = createTable();
    // Not actually sure we need to allocate memory (strdup might do it for us)
    char *label = (char *) malloc(MAX_LINE_LENGTH * sizeof(char));
    int address = 0;
    for (int i = 0; i < numLines; i++) {
        if (strstr(instructionsStrArray[i], ":") != NULL) { // If ':' is in the line
            label = strdup(instructionsStrArray[i]);
            label[strlen(label) - 1] = '\0'; //Add an end of string character to the end of label
            addEntry(symbolTable, label, address * 4);
        } else {
            address++;
        }
    }


    //TODO: Generate binary encoding for each line (Pass 2)
    //Pre: Array of instructions and a adt holding a symbol table
    //Post: An array of binary instructions

    int *instructions = malloc(sizeof(int) * numLines);;

    for (int i = 0; i < numLines; i++) {
        if (strstr(instructionsStrArray[i], ":") == NULL) {
            instructions[i] = processInstruction(instructionsStrArray[i], symbolTable);
        }
    }

    // Save file
    saveToFile(argv[2], instructions);

    return EXIT_SUCCESS;
}
