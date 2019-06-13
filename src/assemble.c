#include <string.h>
#include <stdlib.h>
#include "parser.h"

#define MAX_LINE_LENGTH 511
#define MEMORY_SIZE 65536

unsigned int (*parsers[])(char *, Parser_Data *) = {
        parse_data_processing,
        parse_multiply,
        parse_data_transfer,
        parse_branch,
        parse_special
};

int main(int argc, char **argv) {
    if (argc != 3) {
        perror("Wrong number of arguments");
        return 1;
    }

    FILE *file;
    file = fopen(argv[1], "r");
    char *line = (char *) malloc(sizeof(char) * MAX_LINE_LENGTH);
    int instruction_count = 0;
    Parser_Data *dat = (Parser_Data *) malloc(sizeof(Parser_Data));
    dat->label_table = create_table();
    dat->opcode_table = create_opcode_table();
    dat->parsetype_table = create_parsetype_table();
    dat->memory = (byte *) malloc(sizeof(byte) * MEMORY_SIZE);

    // Generate symbol table (Pass 1)
    char *label;
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strstr(line, ":") != NULL) { // If ':' is in the line
            label = strdup(line);
            label[strlen(label) - 2] = '\0'; // Replace ':' with sentinal character
            add_entry(dat->label_table, label, instruction_count * 4);
        } else if (strcmp(line, "\n")) {
            instruction_count++;
        }
    }

    rewind(file);
    dat->last_instr = instruction_count - 1;
    instruction_count = 0;

    // Generate binary encoding for each line (Pass 2)
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strstr(line, ":") == NULL && strcmp(line, "\n")) {
            dat->cuur_instr = instruction_count;
            char *save;
            dat->pre_indexed = (strstr(line, "],") == NULL);
            dat->mnemonic = strtok_r(line, " ", &save);
            unsigned int processed_instruction = parsers[get_address(dat->parsetype_table, dat->mnemonic)](save, dat);
            int address = instruction_count * 4;
            write_4byte_to_memory(dat->memory, &processed_instruction, &address);
            instruction_count++;
        }
    }

    fclose(file);

    save_to_file(argv[2], dat->memory, &dat->last_instr);

    return 0;
}
