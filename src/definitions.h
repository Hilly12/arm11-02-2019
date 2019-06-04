#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdint.h>

#define MEMORY_SIZE 65536
#define REGISTERS 17
#define PC_REF 15
#define CPSR_REF 16
#define EQ 0
#define NE 1
#define GE 10
#define LT 11
#define GT 12
#define LE 13
#define AL 14
#define ANDEQ 0
#define INVALID 1
#define PROCESSING 2
#define MUL 3
#define MLA 4
#define TRANSFER 5
#define BRANCH 6
#define AND 0
#define EOR 1
#define SUB 2
#define RSB 3
#define ADD 4
#define TST 8
#define TEQ 9
#define CMP 10
#define ORR 12
#define MOV 13
#define LSL 0
#define LSR 1
#define ASR 2
#define ROR 3

typedef struct DecodedInstruction {
    uint8_t type; // Processing, Mul, Mla, Transferring, Branch
    uint8_t Rd; // For processing, multiplying and transferring
    uint8_t Rn; // For processing, multiplying and transferring
    union {
        uint8_t opcode; // For processing
        uint8_t Rm; // For multiplying
        uint8_t L; // For transferring
    };
    union {
        uint8_t carry; // For processing
        uint8_t Rs; // For multiplying
        uint8_t U; // For transferring
    };
    union {
        uint8_t S; // For processing and multiplying
        uint8_t P; // For transferring
    };
    union {
        uint32_t operand2; // For processing
        uint32_t transferOffset; // For transferring
        int32_t branchOffset; // For branch
    };
} DecodedInstruction;

#endif