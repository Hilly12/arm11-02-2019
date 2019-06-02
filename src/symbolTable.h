#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct entry Entry;

typedef struct symbolTable SymbolTable;

SymbolTable *createTable(void);

void addEntry(SymbolTable *symTable, char *label, uint32_t address);

uint32_t getAddress(SymbolTable const *symTable, char *label);

#endif