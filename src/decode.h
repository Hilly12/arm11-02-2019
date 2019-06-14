#ifndef DECODE_H
#define DECODE_H

#include "emulate_utils.h"

typedef struct decoded_instruction {
    byte type; // Processing, Multiplying, Transferring, Branch
    byte Rd; // For processing, multiplying and transferring
    byte Rn; // For processing, multiplying and transferring
    union {
        byte opcode; // For processing
        byte Rm; // For multiplying
        byte L; // For transferring
    };
    union {
        byte carry; // For processing
        byte Rs; // For multiplying
        byte U; // For transferring
    };
    union {
        byte S; // For processing and multiplying
        byte P; // For transferring
    };
    union {
        unsigned int operand2; // For processing
        byte A; // For multiplying
        unsigned int transfer_offset; // For transferring
        int branch_offset; // For branch
    };
} Decoded_Instruction;

void decode_processing(byte const *instr, unsigned int const *registers, Decoded_Instruction *decoded_instr);

void decode_multiplying(byte const *instr, Decoded_Instruction *decoded_instr);

void decode_transferring(byte const *instr, unsigned int const *registers, Decoded_Instruction *decoded_instr);

void decode_branch(byte const *instr, Decoded_Instruction *decoded_instr);

#endif
