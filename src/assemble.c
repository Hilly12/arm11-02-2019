#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "symbolTable.h"
#include "ioManager.h"

#define MAX_LINE_LENGTH 511
#define MEMORY_SIZE 65536

typedef struct ParserData {
    char *mnemonic;
    uint32_t currInstr;
    uint32_t lastInstr;
    SymbolTable *opcodeTable;
    SymbolTable *labelTable;
    SymbolTable *parseTypeTable;
    uint8_t *memory;
    uint8_t preIndexed;
} ParserData;

uint8_t parseRegister(char *token) {
    char *save;
    char *op = strtok_r(token, "r]", &save);
    return strtol(op, NULL, 10);;
}

int32_t parseExpression(char *token) {
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
    int16_t Offset;

    Cond = 0xe;

    L = (dat->mnemonic[0] == 'l');

    char *token;
    
    token = strtok_r(NULL, ", ", &save);
    Rd = parseRegister(token);

    token = strtok_r(NULL, "[, ]\n", &save);
    U = 1;
    if (token[0] == '=' && L == 1) {
        uint32_t expression = parseExpression(token);
        if (expression > 0xff) {
            dat->lastInstr = dat->lastInstr + 1;
            write4ByteToMemory(dat->memory, expression, dat->lastInstr * 4);
            P = 1;
            I = 0;
            Rn = 0xf; // PC
            Offset = (dat->lastInstr - dat->currInstr) * 4 - 8;
        } else {
            return toMoveInstruction(Rd, 1, expression, 0, 0);
        }
    } else {
        I = 0;
        P = dat->preIndexed;

        Rn = parseRegister(token);

        token = strtok_r(NULL, " ,]\n", &save);
        Offset = 0;
        if (token != NULL) {
            I = (token[0] != '#');
            int32_t expression = parseExpression(token);
            Offset = abs(expression) & 0xffff;
            if (expression < 0) {
                U = 0;
            }
            token = strtok_r(NULL, " ,]\n", &save);
            if (token != NULL) {
                I = 1;
                uint8_t shiftType;
                uint32_t shiftOperand;
                int imm = 0;
                shiftType = getAddress(dat->opcodeTable, token) & 0xf;
                
                token = strtok_r(NULL, " ,]\n", &save);
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
int generateBranchOffset(SymbolTable * symbolTable, char *label, int currInstr) {
    return (((int) getAddress(symbolTable, label)) - currInstr - 8) >> 2;
}

int parseBranch(char* save, ParserData *dat) {
    char *token;
    uint8_t Cond;
    int Offset;
    
    Cond = getAddress(dat->opcodeTable, dat->mnemonic) & 0xf;

    token = strtok_r(NULL, " \n", &save);
    Offset = generateBranchOffset(dat->labelTable, token, dat->currInstr * 4);

    return (Cond << 28) | (0xa << 24) | (Offset & 0xffffff);
}

int parseSpecial(char* save, ParserData *dat) {
    // andeq
    if (strcmp(dat->mnemonic, "andeq") == 0) {
        return 0;
    }

    // lsl

    char *token;
    uint8_t Rn;
    uint16_t Operand;

    token = strtok_r(NULL, ", ", &save);
    Rn = parseRegister(token);

    token = strtok_r(NULL, ", ", &save);
    Operand = parseExpression(token);

    return toMoveInstruction(Rn, 0, Rn, 0, Operand);
}

int processInstruction(char *code, ParserData *dat) {
    char *save;

    // Only relevant for ldr, str
    dat->preIndexed = (strstr(code, "],") == NULL);

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
    ParserData *dat = (ParserData *) malloc(sizeof(ParserData));
    uint8_t *memory = (uint8_t *) malloc(sizeof(uint8_t) * MEMORY_SIZE);

    dat->labelTable = symbolTable;
    dat->opcodeTable = opcodeTable;
    dat->parseTypeTable = parseTypeTable;
    dat->lastInstr = instructionCount - 1;
    dat->memory = memory;

    instructionCount = 0;
    int currentInstruction;

    printf("Data for pass 2 initialized\n");

    while(fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strstr(line, ":") == NULL && strcmp(line, "\n")) {
            dat->currInstr = instructionCount;
            currentInstruction = processInstruction(line, dat);
            write4ByteToMemory(memory, currentInstruction, instructionCount * 4);
            instructionCount++;
        }
    }

    fclose(file);

    printf("Pass 2 finished\n");

    // Save file
    save2File(argv[2], memory, dat->lastInstr);

    return EXIT_SUCCESS;
}
