#include "pipeline.h"

// Divides the instruction into 8 entries of the instr array each containing 4 bits of the instruction
void fetch(uint32_t const *pc_val, uint8_t const *memory, uint8_t *instr) {
    for (int8_t i = 0; i < 4; i++) {
        instr[i << 1] = memory[*pc_val + 3 - i] >> 4;
        instr[(i << 1) + 1] = memory[*pc_val + 3 - i] & 0xf;
    }
}

void decode(uint8_t const *instr, uint32_t const *registers, Decoded_Instruction *decoded_instr) {
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
        decoded_instr->type = ANDEQ;
        return;
    }

    // Checking if the instruction is valid (if the condition is satisfied)
    if (!is_condition_satisfied(&registers[CPSR_REF], &instr[0])) {
        decoded_instr->type = INVALID;
        return;
    }

    // Decoding the instruction if it is valid (finding its type and the relevant operands)
    if (instr[1] < 0x4) { // Data Processing or Multiply
        if (instr[6] == 0x9) { // Multiply
            decode_multiplying(instr, decoded_instr);
        } else { // Data Processing
            decode_processing(instr, registers, decoded_instr);
        }
    } else { // Single Data Transfer or Branch
        if (instr[1] == 0xa) { // Branch
            decode_branch(instr, decoded_instr);
        } else { // Single Data Transfer
            decode_transferring(instr, registers, decoded_instr);
        }
    }
}

void execute(uint32_t *registers, uint8_t *memory, uint8_t *gpio, uint8_t *gpio_on,
             uint8_t *gpio_off, Decoded_Instruction *decoded_instr) {
    switch (decoded_instr->type) {
        case PROCESSING:
            execute_processing(registers, decoded_instr);
            return;
        case MULTIPLYING:
            execute_multiplying(registers, decoded_instr);
            return;
        case TRANSFER:
            execute_transferring(registers, memory, gpio, gpio_on, gpio_off, decoded_instr);
            return;
        case BRANCH:
            execute_branch(&registers[PC_REF], decoded_instr);
            return;
        default: // If the decoded instruction is INVALID or ANDEQ just return
            return;
    }
}
