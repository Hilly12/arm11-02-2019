#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Symbol Table is a linked list which intends to 
// map a string to a 32 bit integer

// Represents a single entry in the symbol table
typedef struct entry {
    char *label;
    int address;
    struct entry *prev;
} Entry;

// Represents the symbol table
typedef struct symbolTable {
    Entry *tail;
    int size;
} SymbolTable;

// Allocates memory for a new symbol table
SymbolTable *createTable(void);

// Adds entry to symbol table
void addEntry(SymbolTable *symTable, char *label, int address);

// Searches symbol table for label and returns corresponding address
int getAddress(SymbolTable const *symTable, char *label);

// Creates symbol table mapping string (instruction mnemonic)
// to corresponding opcode
SymbolTable *createOpcodeTable(void);

// Creates symbol table mapping string (instruction mnemonic)
// to corresponding type of instruction for the parser to parse
SymbolTable *createParseTypeTable(void);

#endif