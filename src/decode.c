#include "decode.h"

void decode_processing(byte const *instr, unsigned int const *registers, Decoded_Instruction *decoded_instr) {
    decoded_instr->type = PROCESSING;
    decoded_instr->opcode = ((instr[BITS_24_TO_27] & GET_BIT_1) << 3) | (instr[BITS_20_TO_23] >> 1);
    decoded_instr->S = instr[BITS_20_TO_23] & GET_BIT_1;
    decoded_instr->Rn = instr[BITS_16_TO_19];
    decoded_instr->Rd = instr[BITS_12_TO_15];
    byte I = (instr[BITS_24_TO_27] >> 1) & GET_BIT_1;
    if (I) {
        // If (I == 1) Operand2 is a rotated extended to 32-bits 8-bit constant
        decoded_instr->operand2 =
                (instr[BITS_4_TO_7] << 4) | instr[BITS_0_TO_3]; // The 8-bit constant extended to 32-bits
        decoded_instr->carry = ((decoded_instr->operand2) >> ((instr[BITS_8_TO_11] << 1) - 1)) & GET_BIT_1;
        // Rotating the constant by a 4-bit constant
        decoded_instr->operand2 = (decoded_instr->operand2 >> (instr[BITS_8_TO_11] << 1)) |
                                  (decoded_instr->operand2 << (32 - (instr[BITS_8_TO_11] << 1)));
    } else {
        // If (I == 0) Operand2 is a shifted register by a 5-bit constant or the last byte of a register
        decoded_instr->operand2 = registers[instr[BITS_0_TO_3]]; // The register we are going to shift
        byte shift_type = (instr[BITS_4_TO_7] >> 1) & GET_FIRST_2_BITS;
        byte shift_by;
        byte bit4 = instr[BITS_4_TO_7] & GET_BIT_1;
        if (bit4) {
            // If (bit4 == 1) shift by the last byte of a register
            shift_by = registers[instr[BITS_8_TO_11]] & GET_FIRST_8_BITS;
        } else {
            // If (bit4 == 0) shift by a 5-bit constant and find the carry
            shift_by = (instr[BITS_8_TO_11] << 1) | (instr[BITS_4_TO_7] >> 3);
        }
        if (shift_by) {
            shift_with_carry(&shift_by, &shift_type, &decoded_instr->operand2, &decoded_instr->carry);
        }
    }
}

void decode_multiplying(byte const *instr, Decoded_Instruction *decoded_instr) {
    decoded_instr->type = MULTIPLYING;
    decoded_instr->A = (instr[BITS_20_TO_23] >> 1) & GET_BIT_1;
    decoded_instr->Rd = instr[BITS_16_TO_19];
    decoded_instr->Rm = instr[BITS_0_TO_3];
    decoded_instr->Rs = instr[BITS_8_TO_11];
    decoded_instr->Rn = instr[BITS_12_TO_15];
    decoded_instr->S = instr[BITS_20_TO_23] & GET_BIT_1;
}

void decode_transferring(byte const *instr, unsigned int const *registers, Decoded_Instruction *decoded_instr) {
    decoded_instr->type = TRANSFER;
    decoded_instr->L = instr[BITS_20_TO_23] & GET_BIT_1;
    decoded_instr->P = instr[BITS_24_TO_27] & GET_BIT_1;
    decoded_instr->U = (instr[BITS_20_TO_23] >> 3) & GET_BIT_1;
    decoded_instr->Rn = instr[BITS_16_TO_19];
    decoded_instr->Rd = instr[BITS_12_TO_15];
    byte I = (instr[BITS_24_TO_27] >> 1) & GET_BIT_1;
    if (I) {
        // If (I == 1) Offset is a shifted register by a 5-bit constant or the last byte of a register
        decoded_instr->transfer_offset = registers[instr[BITS_0_TO_3]];
        byte shift_type = (instr[BITS_4_TO_7] >> 1) & GET_FIRST_2_BITS;
        byte shift_by;
        byte bit4 = instr[BITS_4_TO_7] & GET_BIT_1;
        if (bit4) {
            // If (bit4 == 1) shift by the last byte of a register
            shift_by = registers[instr[BITS_8_TO_11]] & GET_FIRST_8_BITS;
        } else {
            // If (bit4 == 0) shift by a 5-bit constant and find the carry
            shift_by = (instr[BITS_8_TO_11] << 1) | (instr[BITS_4_TO_7] >> 3);
        }
        shift(&shift_by, &shift_type, &decoded_instr->transfer_offset);
    } else {
        // If (I == 0) Offset is a 12-bit constant
        decoded_instr->transfer_offset = (((instr[BITS_8_TO_11] << 4) | instr[BITS_4_TO_7]) << 4) | instr[BITS_0_TO_3];
    }
}

void decode_branch(byte const *instr, Decoded_Instruction *decoded_instr) {
    decoded_instr->type = BRANCH;
    // We take as branch_offset the last 24-bits of the instruction
    decoded_instr->branch_offset = (instr[BITS_20_TO_23] << 20) | (instr[BITS_16_TO_19] << 16) |
                                   (instr[BITS_12_TO_15] << 12) | (instr[BITS_8_TO_11] << 8) |
                                   (instr[BITS_4_TO_7] << 4) | instr[BITS_0_TO_3];
}
