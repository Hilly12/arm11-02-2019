#include "symbol_table.h"

Symbol_Table *create_table(void) {
    Symbol_Table *sym_table = malloc(sizeof(Symbol_Table));
    sym_table->tail = NULL;
    sym_table->size = 0;
    return sym_table;
}

void add_entry(Symbol_Table *sym_table, char *label, int address) {
    Entry *e = malloc(sizeof(Entry));
    e->label = label;
    e->address = address;
    e->prev = sym_table->tail;
    sym_table->tail = e;
    sym_table->size += 1;
}

int get_address(Symbol_Table const *sym_table, char const *label) {
    Entry *current = sym_table->tail;
    while (current != NULL) {
        if (!strcmp(current->label, label)) {
            return current->address;
        }
        current = current->prev;
    }
    return 1;
}

Symbol_Table *create_opcode_table(void) {
    Symbol_Table *opcode_table = create_table();
    add_entry(opcode_table, "and", 0x0);
    add_entry(opcode_table, "eor", 0x1);
    add_entry(opcode_table, "sub", 0x2);
    add_entry(opcode_table, "rsb", 0x3);
    add_entry(opcode_table, "add", 0x4);
    add_entry(opcode_table, "tst", 0x8);
    add_entry(opcode_table, "teq", 0x9);
    add_entry(opcode_table, "cmp", 0xa);
    add_entry(opcode_table, "orr", 0xc);
    add_entry(opcode_table, "mov", 0xd);

    add_entry(opcode_table, "lsl", 0x0);
    add_entry(opcode_table, "lsr", 0x1);
    add_entry(opcode_table, "asr", 0x2);
    add_entry(opcode_table, "ror", 0x3);

    add_entry(opcode_table, "beq", 0x0);
    add_entry(opcode_table, "bne", 0x1);
    add_entry(opcode_table, "bge", 0xa);
    add_entry(opcode_table, "blt", 0xb);
    add_entry(opcode_table, "bgt", 0xc);
    add_entry(opcode_table, "ble", 0xd);
    add_entry(opcode_table, "b", 0xe);

    return opcode_table;
}

Symbol_Table *create_parsetype_table(void) {
    Symbol_Table *parsetype_table = create_table();
    add_entry(parsetype_table, "and", 0);
    add_entry(parsetype_table, "eor", 0);
    add_entry(parsetype_table, "sub", 0);
    add_entry(parsetype_table, "rsb", 0);
    add_entry(parsetype_table, "add", 0);
    add_entry(parsetype_table, "tst", 0);
    add_entry(parsetype_table, "teq", 0);
    add_entry(parsetype_table, "cmp", 0);
    add_entry(parsetype_table, "orr", 0);
    add_entry(parsetype_table, "mov", 0);

    add_entry(parsetype_table, "mul", 1);
    add_entry(parsetype_table, "mla", 1);

    add_entry(parsetype_table, "ldr", 2);
    add_entry(parsetype_table, "str", 2);

    add_entry(parsetype_table, "beq", 3);
    add_entry(parsetype_table, "bne", 3);
    add_entry(parsetype_table, "bge", 3);
    add_entry(parsetype_table, "blt", 3);
    add_entry(parsetype_table, "bgt", 3);
    add_entry(parsetype_table, "ble", 3);
    add_entry(parsetype_table, "b", 3);

    add_entry(parsetype_table, "lsl", 4);
    add_entry(parsetype_table, "andeq", 4);

    return parsetype_table;
}
