#include "utils.h"

// Get nth bit from 32 bit data
uint8_t getBit(uint32_t src, uint8_t amt) {
    return (src >> amt) & 0x1;
}

// Rotate 32 bit data right
uint32_t rotateRight32(uint32_t src, uint32_t amt) {
    return (src >> amt) | (src << (32 - amt));
}

// Check if Cond satisfies CPSR register [Pg. 4 spec]
uint8_t isConditionSatisfied(uint32_t cpsr, uint8_t condition) {
    switch (condition) {
        case 0x0: // eq (Z == 1)
            return getBit(cpsr, 30) == 1;
        case 0x1: // ne (Z == 0)
            return getBit(cpsr, 30) == 0;
        case 0xa: // ge (N == V)
            return getBit(cpsr, 31) == getBit(cpsr, 28);
        case 0xb: // lt (N != V)
            return getBit(cpsr, 31) != getBit(cpsr, 28);
        case 0xc: // gt (Z == 0) && (N == V)
            return (getBit(cpsr, 30) == 0) && (getBit(cpsr, 31) == getBit(cpsr, 28));
        case 0xd: // le (Z == 1) || (N != V)
            return (getBit(cpsr, 30) == 1) && (getBit(cpsr, 31) != getBit(cpsr, 28));
        case 0xe: // al
            return 1;
        default:
            return 0;
    }
}

void barrelShift(uint32_t const *registers, uint16_t input,
                 uint32_t *output, uint8_t *shiftCarry) {
    uint8_t shift = input >> 4;
    uint8_t Rm = input & 0xf;
    uint8_t shiftType = (shift >> 1) & 0x3;
    uint8_t shiftAmount;

    *output = registers[Rm];
    if ((shift & 0x1) == 0) { // shift by constant amount
        shiftAmount = shift >> 3;
    } else { // shift by a register
        shiftAmount = registers[shift >> 4] & 0xff;
    }
    if (shiftAmount > 0) {
        switch (shiftType) {
            case 0x0: // lsl
                *shiftCarry = getBit(registers[Rm], 32 - shiftAmount);
                *output = registers[Rm] << shiftAmount;
                break;
            case 0x1: // lsr
                *shiftCarry = getBit(registers[Rm], shiftAmount - 1);
                *output = registers[Rm] >> shiftAmount;
                break;
            case 0x2: // asr
                *shiftCarry = getBit(registers[Rm], shiftAmount - 1);
                *output = (uint32_t) (((int32_t) registers[Rm]) >> shiftAmount);
                break;
            case 0x3: // ror
                *shiftCarry = getBit(registers[Rm], shiftAmount - 1);
                *output = rotateRight32(registers[Rm], shiftAmount);
                break;
            default:
                break;
        }
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