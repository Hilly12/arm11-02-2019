#include "decode.h"

void decode_processing(uint8_t const *instr, uint32_t const *registers, Decoded_Instruction *decoded_instr) {
    decoded_instr->type = PROCESSING;
    decoded_instr->opcode = ((instr[1] & 0x1) << 3) | (instr[2] >> 1);
    decoded_instr->S = instr[2] & 0x1;
    decoded_instr->Rn = instr[3];
    decoded_instr->Rd = instr[4];
    uint8_t I = (instr[1] >> 1) & 0x1;
    if (I) {
        // If (I == 1) Operand2 is a rotated extended to 32-bits 8-bit constant
        decoded_instr->operand2 = (instr[6] << 4) | instr[7]; // The 8-bit constant extended to 32-bits
        decoded_instr->carry = ((decoded_instr->operand2) >> ((instr[5] << 1) - 1)) & 0x1;
        // Rotating the constant by a 4-bit constant
        decoded_instr->operand2 = (decoded_instr->operand2 >> (instr[5] << 1)) |
                                  (decoded_instr->operand2 << (32 - (instr[5] << 1)));
    } else {
        // If (I == 0) Operand2 is a shifted register by a 5-bit constant or the last byte of a register
        decoded_instr->operand2 = registers[instr[7]]; // The register we are going to shift
        uint8_t shift_type = (instr[6] >> 1) & 0x3;
        uint8_t shift_by;
        uint8_t bit4 = instr[6] & 0x1;
        if (bit4) {
            // If (bit4 == 1) shift by the last byte of a register
            shift_by = registers[instr[5]] & 0xff;
        } else {
            // If (bit4 == 0) shift by a 5-bit constant and find the carry
            shift_by = (instr[5] << 1) | (instr[6] >> 3);
        }
        if (shift_by) {
            shift_with_carry(&shift_by, &shift_type, &decoded_instr->operand2, &decoded_instr->carry);
        }
    }
}

void decode_multiplying(uint8_t const *instr, Decoded_Instruction *decoded_instr) {
    decoded_instr->type = MULTIPLYING;
    decoded_instr->A = (instr[2] >> 1) & 0x1;
    decoded_instr->Rd = instr[3];
    decoded_instr->Rm = instr[7];
    decoded_instr->Rs = instr[5];
    decoded_instr->Rn = instr[4];
    decoded_instr->S = instr[2] & 0x1;
}

void decode_transferring(uint8_t const *instr, uint32_t const *registers, Decoded_Instruction *decoded_instr) {
    decoded_instr->type = TRANSFER;
    decoded_instr->L = instr[2] & 0x1;
    decoded_instr->P = instr[1] & 0x1;
    decoded_instr->U = (instr[2] >> 3) & 0x1;
    decoded_instr->Rn = instr[3];
    decoded_instr->Rd = instr[4];
    uint8_t I = (instr[1] >> 1) & 0x1;
    if (I) {
        // If (I == 1) Offset is a shifted register by a 5-bit constant or the last byte of a register
        decoded_instr->transfer_offset = registers[instr[7]];
        uint8_t shift_type = (instr[6] >> 1) & 0x3;
        uint8_t shift_by;
        uint8_t bit4 = instr[6] & 0x1;
        if (bit4) {
            // If (bit4 == 1) shift by the last byte of a register
            shift_by = registers[instr[5]] & 0xff;
        } else {
            // If (bit4 == 0) shift by a 5-bit constant and find the carry
            shift_by = (instr[5] << 1) | (instr[6] >> 3);
        }
        shift(&shift_by, &shift_type, &decoded_instr->transfer_offset);
    } else {
        // If (I == 0) Offset is a 12-bit constant
        decoded_instr->transfer_offset = (((instr[5] << 4) | instr[6]) << 4) | instr[7];
    }
}

void decode_branch(uint8_t const *instr, Decoded_Instruction *decoded_instr) {
    decoded_instr->type = BRANCH;
    // We take as branch_offset the last 24-bits of the instruction
    decoded_instr->branch_offset =
            (((((((((instr[2] << 4) | instr[3]) << 4) | instr[4]) << 4) | instr[5]) << 4) | instr[6]) << 4) | instr[7];
}
