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
    char *save;
    char *op = strtok_r(token, "r#=", &save);
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

    if (Opcode == 0xd) { // mov
        Rn = 0;
    } else {
        token = strtok_r(NULL, ", ", &save);
        Rn = parseRegister(token);
    }
    
    if (0x8 <= Opcode && Opcode <= 0xa) { // tst, teq, cmp
        Rd = 0;
        S = 1;
    } else {
        token = strtok_r(NULL, ", ", &save);
        Rd = parseRegister(token);
        S = 0;
    }

    token = strtok_r(NULL, ", ", &save);
    I = (token[0] == '#');
    Operand = parseOperand(token);

    token = strtok_r(NULL, ", ", &save);
    if (I == 0 && token != NULL) { // Shift
        char *shiftToken, *shiftSave;
        uint8_t shiftType;
        uint16_t shiftOperand;

        shiftToken = strtok_r(token, " ", &shiftSave);
        shiftType = getAddress(dat->opcodeTable, shiftToken) & 0xf;

        shiftToken = strtok_r(NULL, " ", &shiftSave);
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
    
    token = strtok_r(NULL, ", ", &save);
    Rd = parseRegister(token);

    token = strtok_r(NULL, ", ", &save);
    Rm = parseRegister(token);

    token = strtok_r(NULL, ", ", &save);
    Rs = parseRegister(token);
    
    // mla
    if (strcmp(dat->mnemonic, "mla") == 0) {
        token = strtok_r(NULL, ", ", &save);
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
    
    token = strtok_r(NULL, ", ", &save);
    Rd = parseRegister(token);

    token = strtok_r(NULL, ", ", &save);
    Rn = parseRegister(token);
    I = 0;
    P = 0;
    U = 0;
    L = 0;
    Offset = 0;

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

    token = strtok_r(NULL, " ", &save);
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

    return parsers[getAddress(dat->parseTypeTable, dat->mnemonic)](code, save, dat);
}

//Free space allocated to a 2d array
void free2dArray(char **array, unsigned int rows) {
    for (int i = 0; i < rows; i++) {
        free(array[i]);
    }
    free(array);
}

int main(int argc, char **argv) {
    int numLines = 0;


    // Load File into String Array
    char **instructionsStrArray = loadFile(argv, MAX_LINE_LENGTH, &numLines);

    for (int i = 0; i < numLines; i++) {
        printf(instructionsStrArray[i]);
        printf("\n");
    }

    // Generate symbol table (Pass 1)
    SymbolTable *symbolTable = createTable();
    char *label;
    int address = 0;

    for (int i = 0; i < numLines; i++) {
        if (strstr(instructionsStrArray[i], ":") != NULL) { // If ':' is in the line
            label = strdup(instructionsStrArray[i]);
            label[strlen(label) - 1] = '\0'; // Add sentinal character to the end of label
            addEntry(symbolTable, label, address * 4);
        } else {
            address++;
        }
    }
    
    // Generate binary encoding for each line (Pass 2)
    
    SymbolTable *opcodeTable = createOpcodeTable();
    SymbolTable *parseTypeTable = createParseTypeTable();

    int *instructions = (int *) malloc(sizeof(int) * numLines);;
    ParserData *dat = (ParserData *) malloc(sizeof(ParserData));
    dat->labelTable = symbolTable;
    dat->opcodeTable = opcodeTable;
    dat->parseTypeTable = parseTypeTable;
    dat->lastAddress = address * 4;

    for (int i = 0; i < numLines; i++) {
        if (strstr(instructionsStrArray[i], ":") == NULL) {
            dat->currentAddress = i * 4;
            printf(instructionsStrArray[i]);
            printf(": ");
            instructions[i] = processInstruction(instructionsStrArray[i], dat);
            printf("%x\n", instructions[i]);
        }
    }

    // Save file
    saveToFile(argv[2], instructions);
    return EXIT_SUCCESS;
}
