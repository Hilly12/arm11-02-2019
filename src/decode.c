#include "decode.h"

void decodeProcessing(uint8_t const *instr, uint32_t const *registers, DecodedInstruction *decodedInstr) {
    decodedInstr->type = PROCESSING;
    decodedInstr->opcode = ((instr[1] & 0x1) << 3) | (instr[2] >> 1);
    decodedInstr->S = instr[2] & 0x1;
    decodedInstr->Rn = instr[3];
    decodedInstr->Rd = instr[4];
    uint8_t I = (instr[1] >> 1) & 0x1;
    if (I) {
        // If (I == 1) Operand2 is a rotated extended to 32-bits 8-bit constant
        decodedInstr->operand2 = (instr[6] << 4) | instr[7]; // The 8-bit constant extended to 32-bits
        decodedInstr->carry = ((decodedInstr->operand2) >> ((instr[5] << 1) - 1)) & 0x1;
        // Rotating the constant by a 4-bit constant
        decodedInstr->operand2 = (decodedInstr->operand2 >> (instr[5] << 1)) |
                                 (decodedInstr->operand2 << (32 - (instr[5] << 1)));
    } else {
        // If (I == 0) Operand2 is a shifted register by a 5-bit constant or the last byte of a register
        decodedInstr->operand2 = registers[instr[7]]; // The register we are going to shift
        uint8_t shiftType = (instr[6] >> 1) & 0x3;
        uint8_t shiftBy;
        uint8_t bit4 = instr[6] & 0x1;
        if (bit4) {
            // If (bit4 == 1) shift by the last byte of a register
            shiftBy = registers[instr[5]] & 0xff;
        } else {
            // If (bit4 == 0) shift by a 5-bit constant and find the carry
            shiftBy = (instr[5] << 1) | (instr[6] >> 3);
        }
        if (shiftBy) {
            shiftWithCarry(&shiftBy, &shiftType, &decodedInstr->operand2, &decodedInstr->carry);
        }
    }
}

void decodeMultiplying(uint8_t const *instr, DecodedInstruction *decodedInstr) {
    uint8_t A = (instr[2] >> 1) & 0x1;
    if (A) { // A == 1 (mla)
        decodedInstr->type = MLA;
        decodedInstr->Rn = instr[4];
    } else { // A == 0 (mul)
        decodedInstr->type = MUL;
    }
    decodedInstr->Rd = instr[3];
    decodedInstr->Rm = instr[7];
    decodedInstr->Rs = instr[5];
    decodedInstr->S = instr[2] & 0x1;
}

void decodeTransferring(uint8_t const *instr, uint32_t const *registers, DecodedInstruction *decodedInstr) {
    decodedInstr->type = TRANSFER;
    decodedInstr->L = instr[2] & 0x1;
    decodedInstr->P = instr[1] & 0x1;
    decodedInstr->U = (instr[2] >> 3) & 0x1;
    decodedInstr->Rn = instr[3];
    decodedInstr->Rd = instr[4];
    uint8_t I = (instr[1] >> 1) & 0x1;
    if (I) {
        // If (I == 1) Offset is a shifted register by a 5-bit constant or the last byte of a register
        decodedInstr->transferOffset = registers[instr[7]];
        uint8_t shiftType = (instr[6] >> 1) & 0x3;
        uint8_t shiftBy;
        uint8_t bit4 = instr[6] & 0x1;
        if (bit4) {
            // If (bit4 == 1) shift by the last byte of a register
            shiftBy = registers[instr[5]] & 0xff;
        } else {
            // If (bit4 == 0) shift by a 5-bit constant and find the carry
            shiftBy = (instr[5] << 1) | (instr[6] >> 3);
        }
        shift(&shiftBy, &shiftType, &decodedInstr->transferOffset);
    } else {
        // If (I == 0) Offset is a 12-bit constant
        decodedInstr->transferOffset = (((instr[5] << 4) | instr[6] ) << 4) | instr[7];
    }
}

void decodeBranch(uint8_t const *instr, DecodedInstruction *decodedInstr) {
    decodedInstr->type = BRANCH;
    // We take as branchOffset the last 24-bits of the instruction
    decodedInstr->branchOffset =
            (((((((((instr[2] << 4) | instr[3]) << 4) | instr[4]) << 4) | instr[5]) << 4) | instr[6]) << 4) | instr[7];
}