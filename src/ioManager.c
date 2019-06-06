#include "ioManager.h"
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 511

char **init2dCharArray(unsigned int rows, unsigned int cols) {
    char **res = (char **) malloc(rows * sizeof(char *));
    res[0] = (char *) malloc(rows * cols * sizeof(char));
    for (int i = 1; i < rows; i++) {
        res[i] = res[i - 1] + cols;
    }
    return res;
}

char **loadFile(char **argv, int maxLineLength, int *numLines) {
    FILE *file;
    file = fopen(argv[1], "r");

    // Get number of lines
    int lines = 0;
    int ch = 0;
    while (!feof(file)) {
        ch = fgetc(file);
        if ((ch == '\n') || (ch == EOF)) {
            lines++;
        }
    }
    rewind(file);

    *numLines = lines;

    char **array = init2dCharArray(lines, MAX_LINE_LENGTH);

    int i = 0;
    int j = 0;
    while(!feof(file)) {
        ch = fgetc(file);
        if (ch == '\n' || ch == EOF) {
            array[i][j] = '\0';
            i++;
            j = 0;
        } else {
            array[i][j] = ch;
            j++;
        }
    }
    fclose(file);

    return array;
}

BYTE * instructionsToMemory(BYTE *memory, int *instructions) {
    size_t n = sizeof(instructions) / sizeof(int);
    int instrBuffer;
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
    fileOut = fopen(filename, "wb");
    BYTE memory[sizeof(instructions)];
    BYTE *output = instructionsToMemory(memory, instructions);
    fwrite(output, sizeof(memory), 1, fileOut);
    fclose(fileOut);
}