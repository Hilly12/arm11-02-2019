#include "assemble_utils.h"

void write4ByteToMemory(uint8_t *memory, int instruction, int address) {
    int instrBuffer = instruction;
    for (int j = 0; j < 4; j++) {
        memory[address + j] = instrBuffer & 0xff;
        instrBuffer = instrBuffer >> 8;
    }
}

void saveToFile(char *filename, uint8_t *memory, int lastInstr) {
    FILE *file;
    file = fopen(filename, "wb");
    fwrite(memory, (lastInstr + 1) * 4 * sizeof(uint8_t), 1, file);
    fclose(file);
}

int generateBranchOffset(SymbolTable * symbolTable, char *label, int currInstr) {
    return (((int) getAddress(symbolTable, label)) - currInstr - 8) >> 2;
}

int toMoveInstruction(uint8_t Rd, uint8_t I, uint8_t Operand, uint8_t shiftType, uint8_t shiftOperand) {
    return  (0xe << 28) | (I << 25) | (0xd << 21) | (Rd << 12)
            | (((shiftOperand << 7) | (shiftType << 5) | Operand) & 0xfff);
}