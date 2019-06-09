#include "symbol_table.h"
#include "symbol_table.c"

void runSymbolTableTests(void) {
    printf("Starting symbol table debug\n");

    SymbolTable *st = createTable();
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

    printf("Multiple entries added\n");

    printf("Size: %d\n", st->size);
    if (st->size == 5) {
        printf("Correct Size\n");
    }

    printf("eor: %x\n", getAddress(st, "eor"));
    printf("and: %x\n", getAddress(st, "and"));
    printf("rsb: %x\n", getAddress(st, "rsb"));
    printf("add: %x\n", getAddress(st, "add"));
    printf("no'n: %x\n", getAddress(st, "potato"));

    printf("Success\n\n");
}

int main(void) {
    runSymbolTableTests();
    getchar();
    return EXIT_SUCCESS;
}