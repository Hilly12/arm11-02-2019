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

    return opTable;
}

SymbolTable *createParseTypeTable(void) {
    SymbolTable *parseTypeTable = createTable();
    addEntry(parseTypeTable, "and", 0);
    addEntry(parseTypeTable, "eor", 0);
    addEntry(parseTypeTable, "sub", 0);
    addEntry(parseTypeTable, "rsb", 0);
    addEntry(parseTypeTable, "add", 0);
    addEntry(parseTypeTable, "tst", 0);
    addEntry(parseTypeTable, "teq", 0);
    addEntry(parseTypeTable, "cmp", 0);
    addEntry(parseTypeTable, "orr", 0);
    addEntry(parseTypeTable, "mov", 0);

    addEntry(parseTypeTable, "mul", 1);
    addEntry(parseTypeTable, "mla", 1);

    addEntry(parseTypeTable, "ldr", 2);
    addEntry(parseTypeTable, "str", 2);

    addEntry(parseTypeTable, "beq", 3);
    addEntry(parseTypeTable, "bne", 3);
    addEntry(parseTypeTable, "bge", 3);
    addEntry(parseTypeTable, "blt", 3);
    addEntry(parseTypeTable, "bgt", 3);
    addEntry(parseTypeTable, "ble", 3);
    addEntry(parseTypeTable, "b", 3);
    
    addEntry(parseTypeTable, "lsl", 4);
    addEntry(parseTypeTable, "andeq", 4);

    return parseTypeTable;
}