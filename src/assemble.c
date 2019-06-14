#include <string.h>
#include <stdlib.h>
#include "parser.h"

#define MAX_LINE_LENGTH 511
#define MEMORY_SIZE 65536

int main(int argc, char **argv) {
    if (argc != 3) {
        perror("Wrong number of arguments");
        return 1;
    }

    // Loading the file
    FILE *file;
    file = fopen(argv[1], "r");
    char *line = (char *) malloc(sizeof(char) * MAX_LINE_LENGTH);
    int instruction_count = 0;

    // Initializing our Parser_Data structure
    Parser_Data *data = (Parser_Data *) malloc(sizeof(Parser_Data));
    data->label_table = create_table();
    data->mnemonic_table = create_mnemonic_table();
    data->parsetype_table = create_parsetype_table();
    data->memory = (byte *) malloc(sizeof(byte) * MEMORY_SIZE);

    // Generate symbol table (Pass 1) by reading the file line by line
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

    // Function pointers array used to avoid the need of a switch case or multiple if statements
    unsigned int (*parsers[])(char *, Parser_Data *) = {
            parse_data_processing,
            parse_multiply,
            parse_data_transfer,
            parse_branch,
            parse_special
    };

    char *save; // Save pointer to tokenizer
    int parse_type; // Stores index to type of parse function depending on mnemonic
    unsigned int processed_instr; // Stores processed instruction

    // Generate binary encoding for each line (Pass 2)
    while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strstr(line, ":") == NULL && strcmp(line, "\n")) {
            data->curr_instr = instruction_count;
            // Pre-index only relevant for data transfer
            data->pre_indexed = (strstr(line, "],") == NULL); // Pre-indexed if ']' followed by ','
            data->mnemonic = strtok_r(line, " ", &save);
            parse_type = get_address(data->parsetype_table, data->mnemonic);
            processed_instr = parsers[parse_type](save, data);
            write_4byte_to_memory(data->memory, &processed_instr, instruction_count * 4);
            instruction_count++;
        }
    }

    fclose(file);

    save_to_file(argv[2], data->memory, &data->last_instr);

    return 0;
}
