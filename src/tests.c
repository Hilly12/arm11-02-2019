#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "symbolTable.h"
#include "ioManager.h"
#include "ioManager.c"
#include "symbolTable.c"

char **init2dCharArray(int rows, int cols) {
    char **res = (char **) malloc(rows * sizeof(char *));
    res[0] = (char *) malloc(rows * cols * sizeof(char));
    for (int i = 1; i < rows; i++) {
        res[i] = res[i - 1] + cols;
    }
    return res;
}

void runStringToStringArrayTests(void) {

    printf("Starting string to string array debug\n");

    char code[] = "mov r1, #30\nldr r2, [r0]\nadd r1, r1, #20";
    char **code2D = init2dCharArray(3, 511);
    fileToArrayLineByLine(3, code, code2D);

    if (strcmp(code2D[0], "mov r1, #30") == 0) {
        printf("First element checked\n");
    }

    if (strcmp(code2D[1], "ldr r2, [r0]") == 0) {
        printf("Second element checked\n");
    }

    if (strcmp(code2D[2], "add r1, r1, #20") == 0) {
        printf("Third element checked\n");
    }

    printf("Success\n\n");
}

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
    runStringToStringArrayTests();
    getchar();
    return EXIT_SUCCESS;
}