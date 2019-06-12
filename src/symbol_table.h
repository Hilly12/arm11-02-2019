#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct entry {
    char *label;
    uint32_t address;
    struct entry *prev;
} Entry;

typedef struct symbol_table {
    Entry *tail;
    uint32_t size;
} Symbol_Table;

// Allocates memory for a new symbol table
Symbol_Table *create_table(void);

// Adds entry to symbol table
void add_entry(Symbol_Table *sym_table, char *label, int address);

// Searches symbol table for label and returns corresponding address
int get_address(Symbol_Table const *sym_table, char *label);

// Creates symbol table mapping string (instruction mnemonic)
// to corresponding opcode
Symbol_Table *create_opcode_table(void);

// Creates symbol table mapping string (instruction mnemonic)
// to corresponding type of instruction for the parser to parse
Symbol_Table *create_parsetype_table(void);

#endif
