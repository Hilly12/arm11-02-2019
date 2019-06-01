#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdint.h>

#define MEMORY_SIZE 65536
#define REGISTERS 17
#define PC_REF 15
#define CPSR_REF 16

typedef enum instructionType {
    PROCESSING, MULTIPLY, TRANSFER, BRANCH, HALT, INVALID
} instrType;

typedef struct decodedInstruction {
    instrType type;
    uint8_t instruction[8];
} decodedInstruction;

#endif