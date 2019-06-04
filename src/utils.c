#include "utils.h"

// Checks if the sum of 2 integers gives an overflow (the sum is given as an argument as well)
uint8_t checkOverflow(uint32_t const *a, uint32_t const *b, uint32_t const *result) {
    return ((*a > 0 && *b > 0 && ((int32_t) *result) < 0) || (((int32_t) *a) < 0 && ((int32_t) *b) < 0 && *result > 0));
}

// Check if Cond satisfies CPSR register [Pg. 4 spec]
uint8_t isConditionSatisfied(uint32_t const *cpsr, uint8_t const *condition) {
    uint8_t importantBits = *cpsr >> 28;
    switch (*condition) {
        case EQ: // (Z == 1)
            return importantBits & 0x4;
        case NE: // (Z == 0)
            return !(importantBits & 0x4);
        case GE: // (N == V)
            return ((importantBits >> 3) | 0x1) == (importantBits | 0x1);
        case LT: // (N != V)
            return ((importantBits >> 3) | 0x1) != (importantBits | 0x1);
        case GT: // (Z == 0) && (N == V)
            return (!(importantBits & 0x4)) && ((importantBits >> 3) | 0x1) == (importantBits | 0x1);
        case LE: // (Z == 1) || (N != V)
            return (importantBits & 0x4) && ((importantBits >> 3) | 0x1) != (importantBits | 0x1);
        case AL:
            return 1;
        default:
            return 0;
    }
}

void shiftWithCarry(uint8_t const *shiftBy, uint8_t const *shiftType, uint32_t *shiftedValue, uint8_t *shiftCarry) {
    switch (*shiftType) {
        case LSL:
            *shiftCarry = (*shiftedValue >> (32 - *shiftBy)) & 0x1;
            *shiftedValue = *shiftedValue << *shiftBy;
            break;
        case LSR:
            *shiftCarry = (*shiftedValue >> (*shiftBy - 1)) & 0x1;
            *shiftedValue = *shiftedValue >> *shiftBy;
            break;
        case ASR:
            *shiftCarry = (*shiftedValue >> (*shiftBy - 1)) & 0x1;
            *shiftedValue = (uint32_t) (((int32_t) *shiftedValue) >> *shiftBy);
            break;
        case ROR:
            *shiftCarry = (*shiftedValue >> (*shiftBy - 1)) & 0x1;
            *shiftedValue = (*shiftedValue >> *shiftBy) | (*shiftedValue << (32 - *shiftBy));
            break;
        default:
            break;
    }
}

void shift(uint8_t const *shiftBy, uint8_t const *shiftType, uint32_t *shiftedValue) {
    switch (*shiftType) {
        case LSL:
            *shiftedValue = *shiftedValue << *shiftBy;
            break;
        case LSR:
            *shiftedValue = *shiftedValue >> *shiftBy;
            break;
        case ASR:
            *shiftedValue = (uint32_t) (((int32_t) *shiftedValue) >> *shiftBy);
            break;
        case ROR:
            *shiftedValue = (*shiftedValue >> *shiftBy) | (*shiftedValue << (32 - *shiftBy));
            break;
        default:
            break;
    }
}

void processingUpdateCPSR(uint32_t *cpsr, uint32_t const *result, uint8_t const *carry) {
    if (*result) {
        if (*carry) {
            uint8_t bit31 = (*result >> 31) & 0x1;
            if (bit31) {
                *cpsr = (*cpsr & 0xbfffffff) | 0xa0000000; // clears Z and sets N and C of CPSR
            } else {
                *cpsr = (*cpsr & 0x3fffffff) | 0x20000000; // clears N and Z and sets C of CPSR
            }
        } else {
            uint8_t bit31 = (*result >> 31) & 0x1;
            if (bit31) {
                *cpsr = (*cpsr & 0x9fffffff) | 0x80000000; // clears Z and C and sets N of CPSR
            } else {
                *cpsr &= 0x9fffffff; // clears N, Z and C of CPSR
            }
        }
    } else {
        if (*carry) {
            *cpsr = (*cpsr & 0x7fffffff) | 0x60000000; // clears N and sets Z and C of CPSR
        } else {
            *cpsr = (*cpsr & 0x5fffffff) | 0x40000000; // clears N and C and sets Z of CPSR
        }
    }
}

void multiplyingUpdateCPSR(uint32_t *cpsr, uint32_t const *result) {
    if (result) {
        uint8_t bit31 = (*result >> 31) & 0x1;
        if (bit31) {
            *cpsr = (*cpsr & 0x8fffffff) | 0x80000000; // clears Z and sets N of CPSR
        } else {
            *cpsr &= 0x3fffffff; // clears N and Z of CPSR
        }
    } else {
        *cpsr = (*cpsr & 0x7fffffff) | 0x40000000; // clears N and sets Z of CPSR
    }
}

void output(uint32_t const * registers, uint8_t const * memory) {

    // Print registers
    printf("Registers:\n");
    for (int i = 0; i < 13; i++) {
        printf("$%-3d: %10d (0x%08x)\n", i, registers[i], registers[i]);
    }
    printf("PC  : %10d (0x%08x)\n", registers[PC_REF], registers[PC_REF]);
    printf("CPSR: %10d (0x%08x)\n", registers[CPSR_REF], registers[CPSR_REF]);

    // Print non-zero memory
    printf("Non-zero memory:\n");
    for (uint32_t i = 0; i < MEMORY_SIZE; i += 4) {
        int32_t mem = (memory[i] << 24) | (memory[i + 1] << 16)
                      | (memory[i + 2] << 8) | memory[i + 3];
        if (mem != 0) {
            printf("0x%08x: 0x%08x\n", i, mem);
        }
    }
}