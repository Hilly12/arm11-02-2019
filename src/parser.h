#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>
#include <string.h>
#include "assemble_utils.h"

typedef struct Parser_Data {
    char *mnemonic;
    unsigned int cuur_instr;
    unsigned int last_instr;
    Symbol_Table *opcode_table;
    Symbol_Table *label_table;
    Symbol_Table *parsetype_table;
    byte *memory;
    byte pre_indexed;
} Parser_Data;

byte parse_register(char *token);

int parse_expression(char *token);

unsigned int parse_data_processing(char *save, Parser_Data *dat);

unsigned int parse_multiply(char *save, Parser_Data *dat);

unsigned int parse_data_transfer(char *save, Parser_Data *dat);

unsigned int parse_branch(char *save, Parser_Data *dat);

unsigned int parse_special(char *save, Parser_Data *dat);

#endif
