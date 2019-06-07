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
    int *instructions;
} ParserData;

uint8_t parseRegister(char *token) {
    uint8_t r = token[1] - '0';
    if (token[2] == '\0') {
        return r;
    }
    return r * 10 + (token[2] - '0');
}

uint32_t parseExpression(char *token) {
    char *save;
    char *op = strtok_r(token, "r#=", &save);
    if (strstr(token, "x") != NULL) { // Hex
        return strtol(op, NULL, 16);
    } else { // Number or Register
        return strtol(op, NULL, 10);
    }
}

// Operand must be less than or equal to 0xFF
int toMoveInstruction(uint8_t Rd, uint8_t I, uint8_t Operand, uint8_t shiftType, uint8_t shiftOperand) {
    return  (0xe << 28) | (I << 25) | (0xd << 21) | (Rd << 12)
            | (((shiftOperand << 7) | (shiftType << 5) | Operand) & 0xfff);
}

int parseDataProcessing(char* save, ParserData *dat) {
    uint8_t Cond, Opcode, Rn, Rd, I, S;
    uint32_t Operand;

    Cond = 0xe;
    
    // TODO: add mnemonic to symbol table at start of program
    Opcode = getAddress(dat->opcodeTable, dat->mnemonic) & 0xf;

    char *token;
    
    if (0x8 <= Opcode && Opcode <= 0xa) { // tst, teq, cmp
        Rd = 0;
        S = 1;
    } else {
        token = strtok_r(NULL, ", ", &save);
        Rd = parseRegister(token);
        S = 0;
    }

    if (Opcode == 0xd) { // mov
        Rn = 0;
    } else {
        token = strtok_r(NULL, ", ", &save);
        Rn = parseRegister(token);
    }

    token = strtok_r(NULL, ", \n", &save);
    I = (token[0] == '#');
    Operand = parseExpression(token);

    if (I == 1) {
        // Rotate Operand if bigger than 8 bits
        if (Operand > 0xff) {
            uint8_t Rotate = 0;
            while (Operand > 0xff) {
                Operand = (Operand << 2) | (Operand >> 30);
                Rotate++;
                if (Rotate > 0xf) {
                    perror("Cannot be represented");
                    break;
                }
            }
            Operand = (Rotate << 8) | (Operand & 0xff);
        }
    } else {
        // Check for shift input
        token = strtok_r(NULL, ", ", &save);
        if (token != NULL) {
            uint8_t shiftType;
            uint32_t shiftOperand;
            int imm = 0;

            shiftType = getAddress(dat->opcodeTable, token) & 0xf;

            token = strtok_r(NULL, " \n", &save);
            imm = (token[0] == '#');
            shiftOperand = parseExpression(token);

            if (imm) {
                Operand = ((shiftOperand << 7) | (shiftType << 5) | Operand) & 0xfff;
            } else {
                Operand = ((shiftOperand << 8) | (shiftType << 5) | (1 << 4) | Operand) & 0xfff;
            }
        }
    }

    return (Cond << 28) | (I << 25) | (Opcode << 21) 
            | (S << 20) | (Rn << 16) | (Rd << 12) | Operand;
}

int parseMultiply(char* save, ParserData *dat) {
    uint8_t Cond, A, S, Rd, Rn, Rs, Rm;

    Cond = 0xe;

    S = 0;

    char *token;
    
    token = strtok_r(NULL, ", ", &save);
    Rd = parseRegister(token);

    token = strtok_r(NULL, ", ", &save);
    Rm = parseRegister(token);

    token = strtok_r(NULL, ", \n", &save);
    Rs = parseRegister(token);
    
    // mla
    if (strcmp(dat->mnemonic, "mla") == 0) {
        token = strtok_r(NULL, " \n", &save);
        Rn = parseRegister(token);
        A = 1;
    } else {
        Rn = 0;
        A = 0;
    }

    return (Cond << 28) | (A << 21) | (S << 20) | (Rd << 16)
            | (Rn << 12) | (Rs << 8) | (0x9 << 4) | Rm;
}

int parseDataTransfer(char* save, ParserData *dat) {
    uint8_t Cond, I, P, U, L, Rn, Rd;
    uint16_t Offset;

    Cond = 0xe;

    L = (dat->mnemonic[0] == 'l');

    char *token;
    
    token = strtok_r(NULL, ", ", &save);
    Rd = parseRegister(token);

    token = strtok_r(NULL, " \n", &save);
    if (token[0] == '=' && L == 1) {
        uint32_t expression = parseExpression(token);
        if (expression > 0xff) {
            dat->lastAddress += 4;
            dat->instructions[dat->lastAddress / 4] = expression;
            P = 1;
            U = 0;
            I = 0;
            Offset = (dat->lastAddress - dat->currentAddress) + 8;
        } else {
            return toMoveInstruction(Rd, 1, expression, 0, 0);
        }
    } else {
        char *subToken = strtok_r(NULL, "[]", &save), *subSave;

        token = strtok_r(subToken, ", ", &subSave);
        Rn = parseRegister(token);

        token = strtok_r(NULL, ", ", &subSave);
        Offset = 0;
        if (token != NULL) {
            Offset = parseExpression(token);
            token = strtok_r(NULL, ", ", &subSave);
            if (token != NULL) {
                uint8_t shiftType;
                uint32_t shiftOperand;
                int imm = 0;
                shiftType = getAddress(dat->opcodeTable, token) & 0xf;
                
                token = strtok_r(NULL, " \n", &subSave);
                imm = (token[0] == '#');
                shiftOperand = parseExpression(token);
                if (imm) {
                    Offset = ((shiftOperand << 7) | (shiftType << 5) | Offset) & 0xfff;
                } else {
                    Offset = ((shiftOperand << 8) | (shiftType << 5) | (1 << 4) | Offset) & 0xfff;
                }
            }
        }
    }

    return (Cond << 28) | (1 << 26) | (I << 25) | (P << 24)
            | (U << 23) | (L << 20) | (Rn << 16) | (Rd << 12)
            | Offset;
}

// Generate offset for branch instruction
int generateOffset(SymbolTable * symbolTable, char *label, int currentAddress) {
    return (((int) getAddress(symbolTable, label)) - currentAddress - 8) >> 2;
}

int parseBranch(char* save, ParserData *dat) {
    char *token;
    uint8_t Cond;
    int Offset;
    
    Cond = getAddress(dat->opcodeTable, dat->mnemonic) & 0xf;

    token = strtok_r(NULL, " \n", &save);
    Offset = generateOffset(dat->labelTable, token, dat->currentAddress);

    return (Cond << 28) | (0xa << 24) | Offset;
}

int parseSpecial(char* save, ParserData *dat) {
    // andeq
    if (strcmp(dat->mnemonic, "andeq") == 0) {
        return 0;
    }

    // lsl

    char *token;
    uint8_t Cond, Opcode, Rn, Rd, I, S;
    uint16_t Operand;

    Cond = 0xe;
    Opcode = 0xd;
    S = 0;

    token = strtok_r(NULL, ", ", &save);
    Rn = parseRegister(token);
    Rd = 0;

    token = strtok_r(NULL, ", ", &save);
    I = (token[0] == '#');
    Operand = parseExpression(token);

    return (Cond << 28) | (I << 25) | (Opcode << 21) 
            | (S << 20) | (Rn << 16) | (Rd << 12) | Operand;
}

int processInstruction(char *code, ParserData *dat) {
    char *save;
    dat->mnemonic = strtok_r(code, " ", &save);

    int (*parsers[]) (char*, ParserData*) = { 
        parseDataProcessing,
        parseMultiply,
        parseDataTransfer,
        parseBranch,
        parseSpecial
    };

    return parsers[getAddress(dat->parseTypeTable, dat->mnemonic)](save, dat);
}

//Free space allocated to a 2d array
void free2dArray(char **array, int rows) {
    for (int i = 0; i < rows; i++) {
        free(array[i]);
    }
    free(array);
}

int main(int argc, char **argv) {

    if (argc != 3) {
        perror("Wrong number of arguments");
        return 1;
    }

    FILE *file;
    file = fopen(argv[1], "r");

    // Generate symbol table (Pass 1)

    char *label;
    char *line = (char *) malloc (sizeof(char) * MAX_LINE_LENGTH);
    
    int instructionCount = 0;
    SymbolTable *symbolTable = createTable();
    while(fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strstr(line, ":") != NULL) { // If ':' is in the line
            label = strdup(line);
            label[strlen(label) - 2] = '\0'; // Replace ':' with sentinal character
            addEntry(symbolTable, label, instructionCount * 4);
        } else if (strcmp(line, "\n")) {
            instructionCount++;
        }
    }

    rewind(file);

    printf("Pass 1 finished\n");
    
    // Generate binary encoding for each line (Pass 2)
    
    SymbolTable *opcodeTable = createOpcodeTable();
    SymbolTable *parseTypeTable = createParseTypeTable();

    int *instructions = (int *) malloc(sizeof(int) * instructionCount);
    ParserData *dat = (ParserData *) malloc(sizeof(ParserData));
    dat->labelTable = symbolTable;
    dat->opcodeTable = opcodeTable;
    dat->parseTypeTable = parseTypeTable;
    dat->lastAddress = instructionCount * 4;
    dat->instructions = instructions;

    instructionCount = 0;

    printf("Data for pass 2 initialized\n");

    while(fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strstr(line, ":") == NULL && strcmp(line, "\n")) {
            dat->currentAddress = instructionCount * 4;
            instructions[instructionCount] = processInstruction(line, dat);
            instructionCount++;
        }
    }

    fclose(file);

    printf("Pass 2 finished\n");

    // Save file
    saveToFile(argv[2], instructions, instructionCount);

    return EXIT_SUCCESS;
}
