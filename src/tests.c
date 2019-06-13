#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "symbol_table.h"

char **init_2d_char_array(int rows, int cols) {
    char **res = (char **) malloc(rows * sizeof(char *));
    res[0] = (char *) malloc(rows * cols * sizeof(char));
    for (int i = 1; i < rows; i++) {
        res[i] = res[i - 1] + cols;
    }
    return res;
}

//Load data into 2d array
void file_to_array_line_by_line(int num_lines, char *data, char **lines) {
    char *save;
    lines[0] = strtok_r(data, "\n", &save);
    for (int i = 1; i < num_lines; i++) {
        lines[i] = strtok_r(NULL, "\n", &save);
    }
}


void run_string_to_string_array_tests(void) {

    printf("Starting string to string array debug\n");

    char code[] = "mov r1, #30\nldr r2, [r0]\nadd r1, r1, #20";
    char **code2D = init_2d_char_array(3, 511);
    file_to_array_line_by_line(3, code, code2D);

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

void run_symbol_table_tests(void) {
    printf("Starting symbol table debug\n");

    Symbol_Table *st = create_table();
    printf("Table created\n");

    add_entry(st, "and", AND);
    printf("Entry 1 added\n");

    add_entry(st, "eor", EOR);
    printf("Entry 2 added\n");

    add_entry(st, "sub", SUB);
    printf("Entry 3 added\n");

    add_entry(st, "rsb", RSB);
    printf("Entry 4 added\n");

    add_entry(st, "add", ADD);
    printf("Entry 5 added\n");

    printf("Multiple entries added\n");

    printf("Size: %d\n", st->size);
    if (st->size == 5) {
        printf("Correct Size\n");
    }

    printf("eor: %x\n", get_address(st, "eor"));
    printf("and: %x\n", get_address(st, "and"));
    printf("rsb: %x\n", get_address(st, "rsb"));
    printf("add: %x\n", get_address(st, "add"));
    printf("no'n: %x\n", get_address(st, "potato"));

    printf("Success\n\n");
}

void run_parse_tests(void) {
    printf("Starting parser debug\n");

    // char code[] = "add r1 r2 r3";
}

int main(void) {
    run_symbol_table_tests();
    run_string_to_string_array_tests();
    run_parse_tests();
    getchar();
    return EXIT_SUCCESS;
}
