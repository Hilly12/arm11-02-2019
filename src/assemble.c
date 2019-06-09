#include <string.h>
#include <stdlib.h>
#include "parser.h"

#define MAX_LINE_LENGTH 511
#define MEMORY_SIZE 65536

int processInstruction(char *code, ParserData *dat) {
    char *save;

    // Only relevant for ldr, str (could make more robust)
    dat->preIndexed = (strstr(code, "],") == NULL);

    dat->mnemonic = strtok_r(code, " ", &save);

    int (*parsers[]) (char*, ParserData*) = { 
        parseDataProcessing,
        parseMultiply,
        parseDataTransfer,
        parseBranch,
        parseSpecial
    };

    return parsers[getAddress(dat->parseTypeTable, dat->mnemonic)](save, dat);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        perror("Wrong number of arguments");
        return 1;
    }

    FILE *file;
    file = fopen(argv[1], "r");
    char *line = (char *) malloc (sizeof(char) * MAX_LINE_LENGTH);
    int instructionCount = 0;
    SymbolTable *symbolTable = createTable();
    SymbolTable *opcodeTable = createOpcodeTable();
    SymbolTable *parseTypeTable = createParseTypeTable();
    uint8_t *memory = (uint8_t *) malloc(sizeof(uint8_t) * MEMORY_SIZE);
    ParserData *dat = (ParserData *) malloc(sizeof(ParserData));
    dat->labelTable = symbolTable;
    dat->opcodeTable = opcodeTable;
    dat->parseTypeTable = parseTypeTable;
    dat->memory = memory;

    // Generate symbol table (Pass 1)
    char *label;
    while(fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strstr(line, ":") != NULL) { // If ':' is in the line
            label = strdup(line);
            label[strlen(label) - 2] = '\0'; // Replace ':' with sentinal character
            addEntry(symbolTable, label, instructionCount * 4);
        } else if (strcmp(line, "\n")) {
            instructionCount++;
        }
    }

    rewind(file);
    dat->lastInstr = instructionCount - 1;
    instructionCount = 0;
    
    // Generate binary encoding for each line (Pass 2)
    while(fgets(line, MAX_LINE_LENGTH, file) != NULL) {
        if (strstr(line, ":") == NULL && strcmp(line, "\n")) {
            dat->currInstr = instructionCount;
            write4ByteToMemory(memory, processInstruction(line, dat), instructionCount * 4);
            instructionCount++;
        }
    }

    fclose(file);

    saveToFile(argv[2], memory, dat->lastInstr);

    return 0;
}
