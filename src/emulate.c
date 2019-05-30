#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MEMORY_SIZE 65536
#define REGISTERS 17
#define PC_REF 15
#define CPSR_REF 16

enum instructionType {
    PROCESSING, MULTIPLY, TRANSFER, BRANCH, HALT, INVALID
};

struct decodedInstruction {
    enum instructionType type;
    uint8_t instruction[8];

};

void fetch(uint32_t pcVal, uint8_t const *memory, uint8_t *instr) {
    for (int i = 0; i < 4; i++) {
        instr[i * 2] = memory[pcVal + 3 - i] >> 4;
        instr[i * 2 + 1] = memory[pcVal + 3 - i] & 0b1111;
    }
}

void decode(uint8_t const *instr, struct decodedInstruction *decodedInstr) {
    int cnt = 0;
    for (int i = 0; i < 8; i++) {
        decodedInstr->instruction[i] = instr[i];
        if (instr[i] == 0) {
            cnt++;
        }
    }
    if (cnt == 8) {
        decodedInstr->type = HALT;
        return;
    }
    if (instr[1] < 0b0100) { // Data Processing or Multiply
        if (instr[6] == 0b1001) { // Multiply
            decodedInstr->type = MULTIPLY;
        } else { // Data Processing
            decodedInstr->type = PROCESSING;
        }
    } else { // Single Data Transfer or Branch
        if (instr[1] == 0b1010) { // Branch
            decodedInstr->type = BRANCH;
        } else { // Single Data Transfer
            decodedInstr->type = TRANSFER;
        }
    }
}

// Get nth bit from 32 bit data
uint8_t getBit(uint32_t src, uint8_t amt) {
    return (src >> amt) & 0b1;
}

// Rotate 32 bit data right
uint32_t rotateRight32(uint32_t src, uint32_t amt) {
    return (src >> amt) | (src << (32 - amt));
}

// Check if Cond satisfies CPSR register [Pg. 4 spec]
uint8_t isConditionSatisfied(uint32_t cpsr, uint8_t condition) {
    switch (condition) {
        case 0: // eq (Z == 1)
            return getBit(cpsr, 30) == 1;
        case 1: // ne (Z == 0)
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
    uint8_t shiftType = (shift >> 1) & 0b11;
    uint8_t shiftAmount;

    *output = registers[Rm];
    if ((shift & 0b1) == 0) { // shift by constant amount
        shiftAmount = shift >> 3;
    } else { // shift by a register
        shiftAmount = registers[shift >> 4] & 0xff;
    }
    if (shiftAmount > 0) {
        switch (shiftType) {
            case 0: // lsl
                *shiftCarry = getBit(registers[Rm], 32 - shiftAmount);
                *output = registers[Rm] << shiftAmount;
                break;
            case 1: // lsr
                *shiftCarry = getBit(registers[Rm], shiftAmount - 1);
                *output = registers[Rm] >> shiftAmount;
                break;
            case 2: // asr
                *shiftCarry = getBit(registers[Rm], shiftAmount - 1);
                int32_t temp = registers[Rm];
                temp = temp >> shiftAmount;
                *output = (uint32_t) temp;
                break;
            case 3: // ror
                *shiftCarry = getBit(registers[Rm], shiftAmount - 1);
                *output = rotateRight32(registers[Rm], shiftAmount);
                break;
            default:
                break;
        }
    }
}

void executeDataProcessing(uint32_t *registers, uint8_t const *instr) {
    uint8_t I = (instr[1] >> 1) & 0b1;
    uint8_t Opcode = ((instr[1] & 0b1) << 3) | (instr[2] >> 1);
    uint8_t S = instr[2] & 0b1;
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
        case 0b0000: // and
            result = registers[Rn] & Op2;
            registers[Rd] = result;
            carry = shiftCarry;
            break;
        case 0b0001: // eor
            result = registers[Rn] ^ Op2;
            registers[Rd] = result;
            carry = shiftCarry;
            break;
        case 0b0010: // sub
            result = registers[Rn] - Op2;
            registers[Rd] = result;
            if (Op2 <= registers[Rn]) {
                carry = 1;
            }
            break;
        case 0b0011: // rsb
            result = Op2 - registers[Rn];
            registers[Rd] = result;
            if (registers[Rn] <= Op2) {
                carry = 1;
            }
            break;
        case 0b0100: // add
            result = registers[Rn] + Op2;
            registers[Rd] = result;
            if (getBit(registers[Rn], 31) == getBit(Op2, 31)
                && getBit(result, 31) != getBit(Op2, 31)) {
                carry = 1;
            }
            break;
        case 0b1000: // tst
            result = registers[Rn] & Op2;
            carry = shiftCarry;
            break;
        case 0b1001: // teq
            result = registers[Rn] ^ Op2;
            carry = shiftCarry;
            break;
        case 0b1010: // cmp
            result = registers[Rn] - Op2;
            if (Op2 <= registers[Rn]) {
                carry = 1;
            }
            break;
        case 0b1100: // orr
            result = registers[Rn] | Op2;
            registers[Rd] = result;
            carry = shiftCarry;
            break;
        case 0b1101:
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
    uint8_t A = (instr[2] >> 1) & 0b1;
    uint8_t S = instr[2] & 0b1;
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
    uint8_t I = (instr[1] >> 1) & 0b1;
    uint8_t P = instr[1] & 0b1;
    uint8_t U = (instr[2] >> 3) & 0b1;
    uint8_t L = instr[2] & 0b1;
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
    uint8_t sign = (instr[2] >> 3) & 0b1;
    uint32_t destination = (sign == 1) ? (offset | 0xff000000) : offset;

    destination = destination << 2;

    registers[PC_REF] += destination;
}

void execute(uint32_t *registers, uint8_t *memory,
             struct decodedInstruction decodedInstr,
             uint8_t *branched, uint8_t *finished) {

    if (decodedInstr.type == HALT) {
        *finished = 1;
        return;
    }

    uint8_t cond = decodedInstr.instruction[0];
    if (!isConditionSatisfied(registers[CPSR_REF], cond)) {
        return;
    }
    *branched = 0;
    switch (decodedInstr.type) {
        case PROCESSING:
            executeDataProcessing(registers, decodedInstr.instruction);
            break;
        case MULTIPLY:
            executeMultiply(registers, decodedInstr.instruction);
            break;
        case TRANSFER:
            executeSingleDataTransfer(registers, memory, decodedInstr.instruction);
            break;
        case BRANCH:
            *branched = 1;
            executeBranch(registers, decodedInstr.instruction);
            break;
        default:
            break;
    }
}

int main(int argc, char **argv) {

    uint32_t registers[REGISTERS];

    uint8_t memory[MEMORY_SIZE];

    // State of ARM machine when turned on [Pg. 3 Spec]
    for (int i = 0; i < REGISTERS; i++) {
        registers[i] = 0;
    }
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }

    // LOAD BINARY FILE INTO MEMORY
    FILE *binary = fopen(argv[1], "r");
    fread(memory, sizeof(uint8_t), MEMORY_SIZE, binary);
    fclose(binary);

    uint8_t fetchedInstr[8]; // Cond | 4 | 4 | r1 | r2 | 4 | 4 | 4
    struct decodedInstruction decodedInstr;
    decodedInstr.type = INVALID; // initialise

    uint8_t fetched = 0;
    uint8_t decoded = 0;
    uint8_t finished = 0;
    uint8_t branched = 0;

    while (registers[PC_REF] < MEMORY_SIZE) {
        // EXECUTE INSTRUCTION
        if (decoded) {
            execute(registers, memory, decodedInstr, &branched, &finished);
            decoded = 0;
            if (branched) {
                fetched = 0;
            }
        }

        // DECODE INSTRUCTION
        if (fetched) {
            decode(fetchedInstr, &decodedInstr);
            fetched = 0;
            decoded = 1;
        }

        // FETCH INSTRUCTION
        if (!finished) {
            fetch(registers[PC_REF], memory, fetchedInstr);
            fetched = 1;

            // NEXT INSTRUCTION
            registers[PC_REF] += 4;
        }

        if (finished && !decoded) {
            break;
        }
    }

    // OUTPUT

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

    return 0;
}
