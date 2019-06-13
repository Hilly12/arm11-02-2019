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
    Parser_Data *data = (Parser_Data *) malloc(sizeof(Parser_Data));
    data->label_table = create_table();
    data->opcode_table = create_opcode_table();
    data->parsetype_table = create_parsetype_table();
    data->memory = (byte *) malloc(sizeof(byte) * MEMORY_SIZE);

    // Generate symbol table (Pass 1)
    char *label;
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strstr(line, ":") != NULL) { // If ':' is in the line
            label = strdup(line);
            label[strlen(label) - 2] = '\0'; // Replace ':' with sentinal character
            add_entry(data->label_table, label, instruction_count * 4);
        } else if (strcmp(line, "\n")) {
            instruction_count++;
        }
    }

    rewind(file);
    data->last_instr = instruction_count - 1;
    instruction_count = 0;

    // Generate binary encoding for each line (Pass 2)
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strstr(line, ":") == NULL && strcmp(line, "\n")) {
            data->curr_instr = instruction_count;
            char *save;
            data->pre_indexed = (strstr(line, "],") == NULL);
            data->mnemonic = strtok_r(line, " ", &save);
            unsigned int processed_instr = parsers[get_address(data->parsetype_table, data->mnemonic)](save, data);
            int address = instruction_count * 4;
            write_4byte_to_memory(data->memory, &processed_instr, &address);
            instruction_count++;
        }
    }

    fclose(file);

    save_to_file(argv[2], data->memory, &data->last_instr);

    return 0;
}
