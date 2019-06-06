#include "ioManager.h"
#include <stdlib.h>
#include <string.h>


//Load file into the program and get number of lines
char * loadFile(FILE *file, int maxLineLength, int *numLines) {
    //Get number of lines
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

    //Load data
    char * data = (char *) malloc (maxLineLength * lines * sizeof(char));
    fread(data, sizeof(char), lines * maxLineLength, file);
    fclose(file);

    return data;
}

//Load data into 2d array
void fileToArrayLineByLine(int numLines, char *data, char **lines) {
    char *save;
    lines[0] = strtok_r(data, "\n", &save);  
    for (int i = 1; i < numLines; i++) {
        lines[i] = strtok_r(NULL, "\n", &save);
    }
}

//Turns 32 bit instructions to bytes
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

//Saves instructions to file
void saveToFile(char *filename, int *instructions) {
    FILE *fileOut;
    fileOut = fopen(filename, "wb");
    BYTE memory[sizeof(instructions)];
    BYTE *output = instructionsToMemory(memory, instructions);
    fwrite(output, sizeof(memory), 1, fileOut);
    fclose(fileOut);
}