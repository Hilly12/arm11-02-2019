#include "pipeline.h"

// Divides the instruction into 8 entries of the instr array each containing 4 bits of the instruction
void fetch(unsigned int const *pc_val, byte const *memory, byte *instr) {
    for (byte i = 0; i < 4; i++) {
        // 1st iteration: Assigns the value of instr[BITS_28_TO_31]
        // 2nd iteration: Assigns the value of instr[BITS_20_TO_23]
        // 3rd iteration: Assigns the value of instr[BITS_12_TO_15]
        // 4th iteration: Assigns the value of instr[BITS_4_TO_7]
        instr[i << 1] = memory[*pc_val + 3 - i] >> 4;

        // 1st iteration: Assigns the value of instr[BITS_24_TO_27]
        // 2nd iteration: Assigns the value of instr[BITS_16_TO_19]
        // 3rd iteration: Assigns the value of instr[BITS_8_TO_11]
        // 4th iteration: Assigns the value of instr[BITS_0_TO_3]
        instr[(i << 1) + 1] = memory[*pc_val + 3 - i] & GET_FIRST_4_BITS;
    }
}

void decode(byte const *instr, unsigned int const *registers, Decoded_Instruction *decoded_instr) {
    // Checking if the instruction is an andeq
    byte cnt = 0;
    for (byte i = BITS_28_TO_31; i <= BITS_0_TO_3; i++) {
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
    if (!is_condition_satisfied(&registers[CPSR_REF], &instr[BITS_28_TO_31])) {
        decoded_instr->type = INVALID;
        return;
    }

    // Decoding the instruction if it is valid (finding its type and the relevant operands)
    if (instr[BITS_24_TO_27] < 0x4) { // If bits 26 and 27 are both 0 we have Data Processing or Multiplying
        if (instr[BITS_4_TO_7] == 0x9) { //If bits 7-4 are 1,0,0,1 respectively we have Multiplying
            decode_multiplying(instr, decoded_instr);
        } else { // Data Processing
            decode_processing(instr, registers, decoded_instr);
        }
    } else { // Single Data Transfer or Branch
        if (instr[BITS_24_TO_27] == 0xa) { //If bits 27-24 are 1,0,1,0 respectively we have Branch
            decode_branch(instr, decoded_instr);
        } else { // Single Data Transfer
            decode_transferring(instr, registers, decoded_instr);
        }
    }
}

void execute(unsigned int *registers, byte *memory, byte *gpio, byte *gpio_on,
             byte *gpio_off, Decoded_Instruction *decoded_instr) {
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
