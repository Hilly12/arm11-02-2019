#include "execute.h"

void executeDataProcessing(uint32_t *registers, uint8_t const *instr) {
    uint8_t I = (instr[1] >> 1) & 0x1;
    uint8_t Opcode = ((instr[1] & 0x1) << 3) | (instr[2] >> 1);
    uint8_t S = instr[2] & 0x1;
    uint8_t Rn = instr[3];
    uint8_t Rd = instr[4];
    uint16_t Operand2 = (instr[5] << 8) | (instr[6] << 4) | instr[7];

    uint8_t shiftCarry = 0;
    uint32_t Op2;

    if (I == 1) {
        uint32_t rotate = (Operand2 >> 8) * 2;
        uint32_t imm = Operand2 & 0xff;

        shiftCarry = getBit(imm, rotate - 1);
        Op2 = rotateRight32(imm, rotate);
    } else {
        barrelShift(registers, Operand2, &Op2, &shiftCarry);
    }

    uint8_t carry = 0;
    uint32_t result = 0;
    switch (Opcode) {
        case 0x0: // and
            result = registers[Rn] & Op2;
            registers[Rd] = result;
            carry = shiftCarry;
            break;
        case 0x1: // eor
            result = registers[Rn] ^ Op2;
            registers[Rd] = result;
            carry = shiftCarry;
            break;
        case 0x2: // sub
            result = registers[Rn] - Op2;
            registers[Rd] = result;
            if (Op2 <= registers[Rn]) {
                carry = 1;
            }
            break;
        case 0x3: // rsb
            result = Op2 - registers[Rn];
            registers[Rd] = result;
            if (registers[Rn] <= Op2) {
                carry = 1;
            }
            break;
        case 0x4: // add
            result = registers[Rn] + Op2;
            registers[Rd] = result;
            if (getBit(registers[Rn], 31) == getBit(Op2, 31)
                && getBit(result, 31) != getBit(Op2, 31)) {
                carry = 1;
            }
            break;
        case 0x8: // tst
            result = registers[Rn] & Op2;
            carry = shiftCarry;
            break;
        case 0x9: // teq
            result = registers[Rn] ^ Op2;
            carry = shiftCarry;
            break;
        case 0xa: // cmp
            result = registers[Rn] - Op2;
            if (Op2 <= registers[Rn]) {
                carry = 1;
            }
            break;
        case 0xc: // orr
            result = registers[Rn] | Op2;
            registers[Rd] = result;
            carry = shiftCarry;
            break;
        case 0xd:
            registers[Rd] = Op2;
            carry = shiftCarry;
            break;
        default:
            break;
    }

    if (S == 1) {
        uint32_t cpsr = (getBit(result, 31) << 3) | ((result == 0) << 2)
                        | (carry << 1) | getBit(registers[CPSR_REF], 28);
        registers[CPSR_REF] = cpsr << 28;
    }
}

void executeMultiply(uint32_t *registers, uint8_t const *instr) {
    uint8_t A = (instr[2] >> 1) & 0x1;
    uint8_t S = instr[2] & 0x1;
    uint8_t Rd = instr[3];
    uint8_t Rn = instr[4];
    uint8_t Rs = instr[5];
    uint8_t Rm = instr[7];

    uint32_t result = registers[Rm] * registers[Rs];
    if (A == 1) {
        result += registers[Rn];
    }
    registers[Rd] = result;

    if (S == 1) {
        uint32_t cpsr = (getBit(result, 31) << 3) | ((result == 0) << 2)
                        | (getBit(registers[CPSR_REF], 29) << 1)
                        | getBit(registers[CPSR_REF], 28);
        registers[CPSR_REF] = cpsr << 28;
    }
}

void executeSingleDataTransfer(uint32_t *registers, uint8_t *memory, uint8_t const *instr) {
    uint8_t I = (instr[1] >> 1) & 0x1;
    uint8_t P = instr[1] & 0x1;
    uint8_t U = (instr[2] >> 3) & 0x1;
    uint8_t L = instr[2] & 0x1;
    uint8_t Rn = instr[3];
    uint8_t Rd = instr[4];
    uint16_t inputOffset = (instr[5] << 8) | (instr[6] << 4) | instr[7];
    uint32_t offset = inputOffset;
    uint32_t address = registers[Rn];
    uint8_t carry;

    if (I == 1) {
        barrelShift(registers, inputOffset, &offset, &carry);
    }

    if (U == 0) {
        offset = ~offset + 1;
    }

    if (P == 1) {
        address += offset;
    } else {
        registers[Rn] = address + offset;
    }

    if (address + 3 > MEMORY_SIZE) {
        printf("Error: Out of bounds memory access at address 0x%08x\n", address);
        return;
    }

    if (L == 1) {
        registers[Rd] = memory[address]
                        | (memory[address + 1] << 8)
                        | (memory[address + 2] << 16)
                        | (memory[address + 3] << 24);
    } else {
        memory[address] = registers[Rd] & 0xff;
        memory[address + 1] = (registers[Rd] >> 8) & 0xff;
        memory[address + 2] = (registers[Rd] >> 16) & 0xff;
        memory[address + 3] = (registers[Rd] >> 24) & 0xff;
    }
}

void executeBranch(uint32_t *registers, uint8_t const *instr) {
    uint32_t offset = (instr[2] << 20) | (instr[3] << 16)
                      | (instr[4] << 12) | (instr[5] << 8)
                      | (instr[6] << 4) | instr[7];
    registers[PC_REF] += (uint32_t) (((int32_t) (offset << 8)) >> 6);
}