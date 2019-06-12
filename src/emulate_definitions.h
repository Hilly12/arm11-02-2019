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
#define MULTIPLYING 3
#define TRANSFER 4
#define BRANCH 5
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
#define GPIOBYTES 12
#define GPIO_ON_OFF_BYTES 4
#define GPIO_0_9_FIRST_BYTE 0x20200000
#define GPIO_0_9_LAST_BYTE 0x20200003
#define GPIO_10_19_LAST_BYTE 0x20200007
#define GPIO_20_29_FIRST_BYTE 0x20200008
#define GPIO_20_29_LAST_BYTE 0x2020000b
#define GPIO_ON_FIRST_BYTE 0x2020001C
#define GPIO_OFF_FIRST_BYTE 0x20200028

typedef struct decoded_instruction {
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
        uint8_t A; // For multiplying
        uint32_t transfer_offset; // For transferring
        int32_t branch_offset; // For branch
    };
} Decoded_Instruction;

#endif
