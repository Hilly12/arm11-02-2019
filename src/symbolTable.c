#include "symbolTable.h"

struct entry {
    char *label;
    uint32_t address;
    struct entry *prev;
};

struct symbolTable {
    Entry *tail;
    uint32_t size;
};

SymbolTable *createTable(void) {
    SymbolTable *symTable = malloc(sizeof(SymbolTable));
    symTable->tail = NULL;
    symTable->size = 0;
    return symTable;
}

void addEntry(SymbolTable *symTable, char *label, uint32_t address) {
    Entry *e = malloc(sizeof(Entry));
    e->label = label;
    e->address = address;

    e->prev = symTable->tail;
    symTable->tail = e;

    symTable->size += 1;

}

int getAddress(SymbolTable const *symTable, char *label) {
    Entry *current = symTable->tail;
    while (current != NULL) {
        if (strcmp(current->label, label) == 0) {
            return current->address;
        }
        current = current->prev;
    }
    perror("Label does not exist");
    return EXIT_FAILURE;
}


/*
#include <stdio.h> // for debugging only

// Debug
void debug(void) {
    printf("Starting symbol table debug\n");

    symbolTable *st = createTable();
    printf("Table created\n");

    addEntry(st, "and", 0x0);
    printf("Entry 1 added\n");

    addEntry(st, "eor", 0x1);
    printf("Entry 2 added\n");

    addEntry(st, "sub", 0x2);
    printf("Entry 3 added\n");

    addEntry(st, "rsb", 0x3);
    printf("Entry 4 added\n");

    addEntry(st, "add", 0x4);
    printf("Entry 5 added\n");

    printf("Multiple entries added\n\n");

    printf("Size: %d\n", st->size);
    if (st->size == 5) {
        printf("Correct Size\n");
    }

    printf("eor: %x\n", getAddress(st, "eor"));
    printf("and: %x\n", getAddress(st, "and"));
    printf("rsb: %x\n", getAddress(st, "rsb"));
    printf("add: %x\n", getAddress(st, "add"));
    printf("no'n: %x\n\n", getAddress(st, "potato"));

    printf("Success");
}
*/