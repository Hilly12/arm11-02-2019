#include "emulate_utils.h"

byte check_overflow(unsigned int const *a, unsigned int const *b, unsigned int const *result) {
    return ((*a > 0 && *b > 0 && ((int) *result) < 0) || (((int) *a) < 0 && ((int) *b) < 0 && *result > 0));
}

byte is_condition_satisfied(unsigned int const *cpsr, byte const *condition) {
    byte important_bits = *cpsr >> 28; // important_bits = NZCV
    switch (*condition) {
        case EQ:
            return important_bits & GET_BIT_3; // (Z == 1)
        case NE:
            return !(important_bits & GET_BIT_3); // (Z == 0)
        case GE:
            return ((important_bits >> 3) & GET_BIT_1) == (important_bits & GET_BIT_1); // (N == V)
        case LT:
            return ((important_bits >> 3) & GET_BIT_1) != (important_bits & GET_BIT_1); // (N != V)
        case GT:
            return (!(important_bits & GET_BIT_3)) &&
                   ((important_bits >> 3) & GET_BIT_1) == (important_bits & GET_BIT_1); // (Z == 0) && (N == V)
        case LE:
            return (important_bits & GET_BIT_3) &&
                   ((important_bits >> 3) & GET_BIT_1) != (important_bits & GET_BIT_1); // (Z == 1) || (N != V)
        case AL:
            return 1;
        default:
            return 0;
    }
}

void shift_with_carry(byte const *shift_by, byte const *shift_type,
                      unsigned int *shifted_value, byte *shift_carry) {
    switch (*shift_type) {
        case LSL:
            *shift_carry = (*shifted_value >> (32 - *shift_by)) & GET_BIT_1;
            *shifted_value = *shifted_value << *shift_by;
            break;
        case LSR:
            *shift_carry = (*shifted_value >> (*shift_by - 1)) & GET_BIT_1;
            *shifted_value = *shifted_value >> *shift_by;
            break;
        case ASR:
            *shift_carry = (*shifted_value >> (*shift_by - 1)) & GET_BIT_1;
            *shifted_value = (unsigned int) (((int) *shifted_value) >> *shift_by);
            break;
        case ROR:
            *shift_carry = (*shifted_value >> (*shift_by - 1)) & GET_BIT_1;
            *shifted_value = (*shifted_value >> *shift_by) | (*shifted_value << (32 - *shift_by));
            break;
        default:
            break;
    }
}

void shift(byte const *shift_by, byte const *shift_type, unsigned int *shifted_value) {
    switch (*shift_type) {
        case LSL:
            *shifted_value = *shifted_value << *shift_by;
            break;
        case LSR:
            *shifted_value = *shifted_value >> *shift_by;
            break;
        case ASR:
            *shifted_value = (unsigned int) (((int) *shifted_value) >> *shift_by);
            break;
        case ROR:
            *shifted_value = (*shifted_value >> *shift_by) | (*shifted_value << (32 - *shift_by));
            break;
        default:
            break;
    }
}

void processing_update_CPSR(unsigned int *cpsr, unsigned int const *result, byte const *carry) {
    if (*result) {
        if (*carry) {
            byte bit31 = (*result >> 31) & GET_BIT_1;
            if (bit31) {
                *cpsr = (*cpsr & BITMASK_CLEAR_Z) | BITMASK_SET_NC; // clears Z and sets N and C of CPSR
            } else {
                *cpsr = (*cpsr & BITMASK_CLEAR_NZ) | BITMASK_SET_C; // clears N and Z and sets C of CPSR
            }
        } else {
            byte bit31 = (*result >> 31) & GET_BIT_1;
            if (bit31) {
                *cpsr = (*cpsr & BITMASK_CLEAR_ZC) | BITMASK_SET_N; // clears Z and C and sets N of CPSR
            } else {
                *cpsr &= BITMASK_CLEAR_NZC; // clears N, Z and C of CPSR
            }
        }
    } else {
        if (*carry) {
            *cpsr = (*cpsr & BITMASK_CLEAR_N) | BITMASK_SET_ZC; // clears N and sets Z and C of CPSR
        } else {
            *cpsr = (*cpsr & BITMASK_CLEAR_NC) | BITMASK_SET_Z; // clears N and C and sets Z of CPSR
        }
    }
}

void multiplying_update_CPSR(unsigned int *cpsr, unsigned int const *result) {
    if (result) {
        byte bit31 = (*result >> 31) & GET_BIT_1;
        if (bit31) {
            *cpsr = (*cpsr & BITMASK_CLEAR_Z) | BITMASK_SET_N; // clears Z and sets N of CPSR
        } else {
            *cpsr &= BITMASK_CLEAR_NZ; // clears N and Z of CPSR
        }
    } else {
        *cpsr = (*cpsr & BITMASK_CLEAR_N) | BITMASK_SET_Z; // clears N and sets Z of CPSR
    }
}

void gpio_access_print(unsigned int *address) {
    if (*address <= GPIO_0_9_LAST_BYTE) {
        printf("One GPIO pin from 0 to 9 has been accessed\n");
    } else if (*address <= GPIO_10_19_LAST_BYTE) {
        printf("One GPIO pin from 10 to 19 has been accessed\n");
    } else {
        printf("One GPIO pin from 20 to 29 has been accessed\n");
    }
}

void load(unsigned int const *address, byte const *memory, unsigned int *Rd_register) {
    *Rd_register = (memory[*address + 3] << 24) | (memory[*address + 2] << 16) |
                   (memory[*address + 1] << 8) | memory[*address];
}

void store(unsigned int const *address, byte *memory, unsigned int const *Rd_register) {
    memory[*address] = *Rd_register & GET_FIRST_8_BITS;
    memory[*address + 1] = (*Rd_register >> 8) & GET_FIRST_8_BITS;
    memory[*address + 2] = (*Rd_register >> 16) & GET_FIRST_8_BITS;
    memory[*address + 3] = (*Rd_register >> 24) & GET_FIRST_8_BITS;
}

void output(unsigned int const *registers, byte const *memory) {
    // Print registers
    printf("Registers:\n");
    for (byte i = 0; i < 13; i++) {
        printf("$%-3d: %10d (0x%08x)\n", i, registers[i], registers[i]);
    }
    printf("PC  : %10d (0x%08x)\n", registers[PC_REF], registers[PC_REF]);
    printf("CPSR: %10d (0x%08x)\n", registers[CPSR_REF], registers[CPSR_REF]);

    // Print non-zero memory
    printf("Non-zero memory:\n");
    for (unsigned int i = 0; i < MEMORY_SIZE; i += 4) {
        int mem = (memory[i] << 24) | (memory[i + 1] << 16) | (memory[i + 2] << 8) | memory[i + 3];
        if (mem != 0) {
            printf("0x%08x: 0x%08x\n", i, mem);
        }
    }
}
