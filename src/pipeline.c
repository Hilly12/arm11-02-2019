#include "pipeline.h"

// Divides the instruction into 8 entries of the instr array each containing 4 bits of the instruction
void fetch(uint32_t const *pcVal, uint8_t const *memory, uint8_t *instr) {
    for (int8_t i = 0; i < 4; i++) {
        instr[i * 2] = memory[*pcVal + 3 - i] >> 4;
        instr[i * 2 + 1] = memory[*pcVal + 3 - i] & 0xf;
    }
}

void decode(uint8_t const *instr, uint32_t const *registers, DecodedInstruction *decodedInstr) {
    // Checking if the instruction is an andeq
    uint8_t cnt = 0;
    for (uint8_t i = 0; i < 8; i++) {
        if (!instr[i]) {
            cnt++;
        } else {
            break;
        }
    }
    if (cnt == 8) {
        decodedInstr->type = ANDEQ;
        return;
    }

    // Checking if the instruction is valid (if the condition is satisfied)
    if (!isConditionSatisfied(&registers[CPSR_REF], &instr[0])) {
        decodedInstr->type = INVALID;
        return;
    }

    // Decoding the instruction if it is valid (finding its type and the relevant operands)
    if (instr[1] < 0x4) { // Data Processing or Multiply
        if (instr[6] == 0x9) { // Multiply
            decodeMultiplying(instr, decodedInstr);
        } else { // Data Processing
            decodeProcessing(instr, registers, decodedInstr);
        }
    } else { // Single Data Transfer or Branch
        if (instr[1] == 0xa) { // Branch
            decodeBranch(instr, decodedInstr);
        } else { // Single Data Transfer
            decodeTransferring(instr, registers, decodedInstr);
        }
    }
}

void execute(uint32_t *registers, uint8_t *memory, uint8_t *gpio, uint8_t *gpio_on, 
        uint8_t *gpio_off, DecodedInstruction *decodedInstr) {
    switch (decodedInstr->type) {
        case PROCESSING:
            executeProcessing(registers, decodedInstr);
            return;
        case MULTIPLYING:
            executeMultiplying(registers, decodedInstr);
            return;
        case TRANSFER:
            executeTransferring(registers, memory, gpio, gpio_on, gpio_off, decodedInstr);
            return;
        case BRANCH:
            executeBranch(&registers[PC_REF], decodedInstr);
            return;
        default: // If the decoded instruction is INVALID or ANDEQ just return
            return;
    }
}
