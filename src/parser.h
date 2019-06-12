#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "assemble_utils.h"

typedef struct Parser_Data {
    char *mnemonic;
    uint32_t cuur_instr;
    uint32_t last_instr;
    Symbol_Table *opcode_table;
    Symbol_Table *label_table;
    Symbol_Table *parsetype_table;
    uint8_t *memory;
    uint8_t pre_indexed;
} Parser_Data;

uint8_t parse_register(char *token);

int parse_expression(char *token);

int parse_data_processing(char *save, Parser_Data *dat);

int parse_multiply(char *save, Parser_Data *dat);

int parse_data_transfer(char *save, Parser_Data *dat);

int parse_branch(char *save, Parser_Data *dat);

int parse_special(char *save, Parser_Data *dat);

#endif
