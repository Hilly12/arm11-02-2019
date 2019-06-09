#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "assemble_utils.h"

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

uint8_t parseRegister(char *token);

int parseExpression(char *token);

int parseDataProcessing(char* save, ParserData *dat);

int parseMultiply(char* save, ParserData *dat);

int parseDataTransfer(char* save, ParserData *dat);

int parseBranch(char* save, ParserData *dat);

int parseSpecial(char* save, ParserData *dat);

#endif