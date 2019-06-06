#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "symbolTable.h"
#include "ioManager.h"

#define MAX_LINE_LENGTH 511

typedef struct ParserData {
    char *mnemonic;
    uint32_t currentAddress;
    uint32_t lastAddress;
    SymbolTable *opcodeTable;
    SymbolTable *labelTable;
    SymbolTable *parseTypeTable;
} ParserData;

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

int parseDataProcessing(char* code, char* save, ParserData *dat) {
    uint8_t Cond, Opcode, Rn, Rd, I, S;
    uint16_t Operand;

    Cond = 0xe;
    
    // TODO: add mnemonic to symbol table at start of program
    Opcode = getAddress(dat->opcodeTable, dat->mnemonic) & 0xf;

    char *token;
    
    if (0x8 <= Opcode && Opcode <= 0x10) { // tst, teq, cmp
        Rn = 0;
        S = 1;
    } else {
        token = strtok_r(code, ", ", &save);
        Rn = parseRegister(token);
        S = 0;
    }

    if (Opcode == 0xd) { // mov
        Rd = 0;
    } else {
        token = strtok_r(code, ", ", &save);
        Rd = parseRegister(token);
    }

    token = strtok_r(code, ", ", &save);
    I = (token[0] == '#');
    Operand = parseOperand(token);

    token = strtok_r(code, ", ", &save);
    if (I == 0 && token != NULL) { // Shift
        char *shiftToken, *shiftSave;
        uint8_t shiftType;
        uint16_t shiftOperand;

        shiftToken = strtok_r(token, " ", &shiftSave);
        shiftType = getAddress(dat->opcodeTable, shiftToken) & 0xf;

        shiftToken = strtok_r(token, " ", &shiftSave);
        shiftOperand = parseOperand(shiftToken);

        if (shiftToken[0] == '#') {
            Operand = ((shiftOperand << 7) | (shiftType << 5) | Operand) & 0xfff;
        } else {
            Operand = ((shiftOperand << 8) | (shiftType << 5) | (1 << 4) | Operand) & 0xfff;
        }
    }

    return (Cond << 28) | (I << 25) | (Opcode << 21) 
            | (S << 20) | (Rn << 16) | (Rd << 12) | Operand;
}

int parseMultiply(char* code, char* save, ParserData *dat) {
    uint8_t Cond, A, S, Rd, Rn, Rs, Rm;

    Cond = 0xe;

    S = 0;

    char *token;
    
    token = strtok_r(code, ", ", &save);
    Rd = parseRegister(token);

    token = strtok_r(code, ", ", &save);
    Rm = parseRegister(token);

    token = strtok_r(code, ", ", &save);
    Rs = parseRegister(token);
    
    // mla
    if (strcmp(dat->mnemonic, "mla") == 0) {
        token = strtok_r(code, ", ", &save);
        Rn = parseRegister(token);
        A = 1;
    } else {
        Rn = 0;
        A = 0;
    }

    return (Cond << 28) | (A << 21) | (S << 20) | (Rd << 16)
            | (Rn << 12) | (Rs << 8) | (0x9 << 4) | Rm;
}

int parseDataTransfer(char* code, char* save, ParserData *dat) {
    uint8_t Cond, I, P, U, L, Rn, Rd;
    uint16_t Offset;

    Cond = 0xe;

    char *token;
    
    token = strtok_r(code, ", ", &save);
    Rn = parseRegister(token);

    token = strtok_r(code, ", ", &save);
    Rd = parseRegister(token);

    return (Cond << 28) | (1 << 26) | (I << 25) | (P << 24)
            | (U << 23) | (L << 20) | (Rn << 16) | (Rd << 12)
            | Offset;
}

// Generate offset for branch instruction
int generateOffset(SymbolTable * symbolTable, char *label, int currentAddress) {
    return (getAddress(symbolTable, label) - currentAddress - 4) >> 2;
}

int parseBranch(char* code, char* save, ParserData *dat) {
    char *token;
    uint8_t Cond;
    uint32_t Offset;
    
    Cond = getAddress(dat->opcodeTable, dat->mnemonic) & 0xf;

    token = strtok_r(code, " ", &save);
    Offset = generateOffset(dat->labelTable, token, dat->currentAddress); // TODO: pass address

    return (Cond << 28) | (0xa << 24) | Offset;
}

int parseSpecial(char* code, char* save, ParserData *dat) {
    // andeq
    if (strcmp(dat->mnemonic, "andeq") == 0) {
        return 0;
    }

    // lsl
    // char *token;
    // uint8_t Cond, Opcode, Rn, Rd, I, S;
    // uint16_t Operand;

    // Cond = 0xe;
    // Opcode = 0xd;
    // S = 0;

    // token = strtok_r(code, ", ", &save);
    // Rn = parseRegister(token);
    // Rd = 0;

    // token = strtok_r(code, ", ", &save);
    // I = (token[0] == '#');
    // Operand = parseOperand(token);

    // return (Cond << 28) | (I << 25) | (Opcode << 21) 
    //         | (S << 20) | (Rn << 16) | (Rd << 12) | Operand;
    return 0;
}

int processInstruction(char *code, ParserData *dat) {
    char *save;
    dat->mnemonic = strtok_r(code, " ", &save);

    int (*parsers[]) (char*, char*, ParserData*) = { 
        parseDataProcessing,
        parseMultiply,
        parseDataTransfer,
        parseBranch,
        parseSpecial
    };

    return parseDataProcessing(code, save, dat);
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
    // NOTE: Not actually sure we need to allocate memory (strdup might do it for us)
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


    // Generate binary encoding for each line (Pass 2)
    
    SymbolTable *opcodeTable = createTable();
    SymbolTable *parseTypeTable = createTable();

    int *instructions = malloc(sizeof(int) * numLines);;
    ParserData *dat = malloc(sizeof(ParserData));
    dat->labelTable = symbolTable;
    dat->lastAddress = address * 4;
    for (int i = 0; i < numLines; i++) {
        if (strstr(instructionsStrArray[i], ":") == NULL) {
            dat->currentAddress = i * 4;
            instructions[i] = processInstruction(instructionsStrArray[i], dat);
        }
    }



    // Save file
    saveToFile(argv[2], instructions);

    return EXIT_SUCCESS;
}
