#include "symbol_table.h"

Symbol_Table *create_table(void) {
    Symbol_Table *sym_table = malloc(sizeof(Symbol_Table));
    sym_table->tail = NULL;
    sym_table->size = 0;
    return sym_table;
}

void add_entry(Symbol_Table *sym_table, char *label, unsigned int address) {
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

Symbol_Table *create_mnemonic_table(void) {
    Symbol_Table *mnemonic_table = create_table();

    add_entry(mnemonic_table, "and", AND);
    add_entry(mnemonic_table, "eor", EOR);
    add_entry(mnemonic_table, "sub", SUB);
    add_entry(mnemonic_table, "rsb", RSB);
    add_entry(mnemonic_table, "add", ADD);
    add_entry(mnemonic_table, "tst", TST);
    add_entry(mnemonic_table, "teq", TEQ);
    add_entry(mnemonic_table, "cmp", CMP);
    add_entry(mnemonic_table, "orr", ORR);
    add_entry(mnemonic_table, "mov", MOV);

    add_entry(mnemonic_table, "lsl", LSL);
    add_entry(mnemonic_table, "lsr", LSR);
    add_entry(mnemonic_table, "asr", ASR);
    add_entry(mnemonic_table, "ror", ROR);

    add_entry(mnemonic_table, "beq", BEQ);
    add_entry(mnemonic_table, "bne", BNE);
    add_entry(mnemonic_table, "bge", BGE);
    add_entry(mnemonic_table, "blt", BLT);
    add_entry(mnemonic_table, "bgt", BGT);
    add_entry(mnemonic_table, "ble", BLE);
    add_entry(mnemonic_table, "b", B);

    return mnemonic_table;
}

Symbol_Table *create_parsetype_table(void) {
    Symbol_Table *parsetype_table = create_table();

    add_entry(parsetype_table, "and", PARSETYPE_0);
    add_entry(parsetype_table, "eor", PARSETYPE_0);
    add_entry(parsetype_table, "sub", PARSETYPE_0);
    add_entry(parsetype_table, "rsb", PARSETYPE_0);
    add_entry(parsetype_table, "add", PARSETYPE_0);
    add_entry(parsetype_table, "tst", PARSETYPE_0);
    add_entry(parsetype_table, "teq", PARSETYPE_0);
    add_entry(parsetype_table, "cmp", PARSETYPE_0);
    add_entry(parsetype_table, "orr", PARSETYPE_0);
    add_entry(parsetype_table, "mov", PARSETYPE_0);

    add_entry(parsetype_table, "mul", PARSETYPE_1);
    add_entry(parsetype_table, "mla", PARSETYPE_1);

    add_entry(parsetype_table, "ldr", PARSETYPE_2);
    add_entry(parsetype_table, "str", PARSETYPE_2);

    add_entry(parsetype_table, "beq", PARSETYPE_3);
    add_entry(parsetype_table, "bne", PARSETYPE_3);
    add_entry(parsetype_table, "bge", PARSETYPE_3);
    add_entry(parsetype_table, "blt", PARSETYPE_3);
    add_entry(parsetype_table, "bgt", PARSETYPE_3);
    add_entry(parsetype_table, "ble", PARSETYPE_3);
    add_entry(parsetype_table, "b", PARSETYPE_3);

    add_entry(parsetype_table, "lsl", PARSETYPE_4);
    add_entry(parsetype_table, "andeq", PARSETYPE_4);

    return parsetype_table;
}
