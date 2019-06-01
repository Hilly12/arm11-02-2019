#include "pipeline.h"

void fetch(uint32_t pcVal, uint8_t const *memory, uint8_t *instr) {
    for (int i = 0; i < 4; i++) {
        instr[i * 2] = memory[pcVal + 3 - i] >> 4;
        instr[i * 2 + 1] = memory[pcVal + 3 - i] & 0xf;
    }
}

void decode(uint8_t const *instr, decodedInstruction *decodedInstr) {
    int cnt = 0;
    for (int i = 0; i < 8; i++) {
        decodedInstr->instruction[i] = instr[i];
        if (instr[i] == 0) {
            cnt++;
        }
    }
    if (cnt == 8) {
        decodedInstr->type = HALT;
        return;
    }
    if (instr[1] < 0x4) { // Data Processing or Multiply
        if (instr[6] == 0x9) { // Multiply
            decodedInstr->type = MULTIPLY;
        } else { // Data Processing
            decodedInstr->type = PROCESSING;
        }
    } else { // Single Data Transfer or Branch
        if (instr[1] == 0xa) { // Branch
            decodedInstr->type = BRANCH;
        } else { // Single Data Transfer
            decodedInstr->type = TRANSFER;
        }
    }
}

void execute(uint32_t *registers, uint8_t *memory,
             decodedInstruction decodedInstr,
             uint8_t *branched, uint8_t *finished) {

    if (decodedInstr.type == HALT) {
        *finished = 1;
        return;
    }

    uint8_t cond = decodedInstr.instruction[0];
    if (!isConditionSatisfied(registers[CPSR_REF], cond)) {
        return;
    }
    *branched = 0;
    switch (decodedInstr.type) {
        case PROCESSING:
            executeDataProcessing(registers, decodedInstr.instruction);
            break;
        case MULTIPLY:
            executeMultiply(registers, decodedInstr.instruction);
            break;
        case TRANSFER:
            executeSingleDataTransfer(registers, memory, decodedInstr.instruction);
            break;
        case BRANCH:
            *branched = 1;
            executeBranch(registers, decodedInstr.instruction);
            break;
        default:
            break;
    }
}