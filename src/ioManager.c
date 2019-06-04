#include "ioManager.h"

void loadFile(char **argv, int maxLineLength, int *numLines, char* data) {
    
    FILE *file;
    file = fopen(argv[1], "r");

    int lines = 0;
    int ch = 0;
    while (!feof(file)) {
        ch = fgetc(file);
        if ((ch == '\n') || (ch == EOF)) {
            lines++;
        }
    }
    rewind(file);

    fread(data, sizeof(char), lines * maxLineLength, file);
    *numLines = lines;
}

void fileToArrayLineByLine(int numLines, FILE *file, char** lines) {
    char *save;
    lines[0] = strtok_r(file, "\n", &save);  
    for (int i = 1; i < numLines; i++) {
        lines[i] = strtok_r(NULL, "\n", &save);
    }
}

uint8_t *instructionsToMemory(uint8_t *memory, uint32_t *instructions) {
    size_t n = sizeof(instructions) / sizeof(uint32_t);
    uint32_t instrBuffer;
    for (int i = 0; i < n; ++i) {
        instrBuffer = instructions[i];

        memory[i * 4] = (instrBuffer & 0xFF);
        memory[i * 4 + 1] = ((instrBuffer >> 8) & 0xFF);
        memory[i * 4 + 2] = ((instrBuffer >> 16) & 0xFF);
        memory[i * 4 + 3] = ((instrBuffer >> 24) & 0xFF);
    }
    return memory;
}

void saveToFile(char *filename, int *instructions) {
    FILE *fileOut;
    fileOut = fopen(filename, "w+");
    uint8_t memory[sizeof(instructions)];
    uint8_t *output = instructionsToMemory(memory, instructions);
    fwrite(output, sizeof(memory), 1, fileOut);
    fclose(fileOut);
}