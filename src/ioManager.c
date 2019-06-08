#include "ioManager.h"
#include <stdlib.h>
#include <string.h>

char **init2dCharArray(int rows, int cols) {
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
    char ch = '\0';
    char lastCh =  '\0';
    while (!feof(file)) {
        lastCh = ch;
        ch = fgetc(file);
        if ((ch == '\n') || (ch == EOF && lastCh != '\n')) {
            lines++;
        }
    }
    rewind(file);

    *numLines = lines;

    char **array = init2dCharArray(lines, maxLineLength);

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

void instructionsToMemory(uint8_t *memory, int const *instructions, int instructionCount) {
    int instrBuffer;
    for (int i = 0; i < instructionCount; i++) {
        instrBuffer = instructions[i];

        for (int j = 0; j < 4; j++) {
            memory[i * 4 + j] = instrBuffer & 0xff;
            instrBuffer = instrBuffer >> 8;
        }
    }
}

void write4ByteToMemory(uint8_t *memory, int instruction, int address) {
    int instrBuffer = instruction;
    for (int j = 0; j < 4; j++) {
        memory[address + j] = instrBuffer & 0xff;
        instrBuffer = instrBuffer >> 8;
    }
}

void saveToFile(char *filename, int const *instructions, int instructionCount) {
    FILE *fileOut;
    fileOut = fopen(filename, "wb");

    int memorySize = instructionCount * 4;

    uint8_t *memory = (uint8_t *) malloc(sizeof(uint8_t) * memorySize);

    instructionsToMemory(memory, instructions, instructionCount);
    // for (int i = 0; i < instructionCount; i++) {
    //     printf("%d: %x\n", i * 4, memory[i * 4]);
    //     printf("%d: %x\n", i * 4 + 1, memory[i * 4 + 1]);
    //     printf("%d: %x\n", i * 4 + 2, memory[i * 4 + 2]);
    //     printf("%d: %x\n", i * 4 + 3, memory[i * 4 + 3]);
    // }
    fwrite(memory, memorySize * sizeof(uint8_t), 1, fileOut);
    fclose(fileOut);
}

void save2File(char *filename, uint8_t *memory, int lastInstr) {
    FILE *file;
    file = fopen(filename, "wb");
    fwrite(memory, (lastInstr + 1) * 4 * sizeof(uint8_t), 1, file);
    fclose(file);
}
