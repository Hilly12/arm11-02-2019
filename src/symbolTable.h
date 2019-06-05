#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Represents a single entry in the symbol table
typedef struct entry Entry;

// Represents the symbol table
typedef struct symbolTable SymbolTable;

// Allocates memory for a new symbol table
SymbolTable *createTable(void);

// Adds entry to symbol table
void addEntry(SymbolTable *symTable, char *label, uint32_t address);

// Searches symbol table for label and returns corresponding address
int getAddress(SymbolTable const *symTable, char *label);

#endif