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

SymbolTable *createOpcodeTable(void) {
    SymbolTable *opTable = createTable();
    addEntry(opTable, "and", 0x0);
    addEntry(opTable, "eor", 0x1);
    addEntry(opTable, "sub", 0x2);
    addEntry(opTable, "rsb", 0x3);
    addEntry(opTable, "add", 0x4);
    addEntry(opTable, "tst", 0x8);
    addEntry(opTable, "teq", 0x9);
    addEntry(opTable, "cmp", 0xa);
    addEntry(opTable, "orr", 0xc);
    addEntry(opTable, "mov", 0xd);

    addEntry(opTable, "lsl", 0x0);
    addEntry(opTable, "lsr", 0x1);
    addEntry(opTable, "asr", 0x2);
    addEntry(opTable, "ror", 0x3);

    addEntry(opTable, "beq", 0x0);
    addEntry(opTable, "bne", 0x1);
    addEntry(opTable, "bge", 0xa);
    addEntry(opTable, "blt", 0xb);
    addEntry(opTable, "bgt", 0xc);
    addEntry(opTable, "ble", 0xd);
    addEntry(opTable, "b", 0xe);
}

SymbolTable *createParseTypeTable(void) {
    SymbolTable *opTable = createTable();
    addEntry(opTable, "and", 0);
    addEntry(opTable, "eor", 0);
    addEntry(opTable, "sub", 0);
    addEntry(opTable, "rsb", 0);
    addEntry(opTable, "add", 0);
    addEntry(opTable, "tst", 0);
    addEntry(opTable, "teq", 0);
    addEntry(opTable, "cmp", 0);
    addEntry(opTable, "orr", 0);
    addEntry(opTable, "mov", 0);

    addEntry(opTable, "mul", 1);
    addEntry(opTable, "mla", 1);

    addEntry(opTable, "ldr", 2);
    addEntry(opTable, "str", 2);

    addEntry(opTable, "beq", 3);
    addEntry(opTable, "bne", 3);
    addEntry(opTable, "bge", 3);
    addEntry(opTable, "blt", 3);
    addEntry(opTable, "bgt", 3);
    addEntry(opTable, "ble", 3);
    addEntry(opTable, "b", 3);
    
    addEntry(opTable, "lsl", 4);
    addEntry(opTable, "andeq", 4);
}