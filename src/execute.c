#include "execute.h"

void executeProcessing(uint32_t *registers, DecodedInstruction *decodedInstr) {
    switch (decodedInstr->opcode) {
        case AND:
            registers[decodedInstr->Rd] =
                    registers[decodedInstr->Rn] & decodedInstr->operand2;
            if (decodedInstr->S) {
                processingUpdateCPSR(&registers[CPSR_REF],
                                     &registers[decodedInstr->Rd],
                                     &decodedInstr->carry);
            }
            return;
        case EOR:
            registers[decodedInstr->Rd] =
                    registers[decodedInstr->Rn] ^ decodedInstr->operand2;
            if (decodedInstr->S) {
                processingUpdateCPSR(&registers[CPSR_REF],
                                     &registers[decodedInstr->Rd],
                                     &decodedInstr->carry);
            }
            return;
        case SUB:
            registers[decodedInstr->Rd] =
                    registers[decodedInstr->Rn] - decodedInstr->operand2;
            if (decodedInstr->S) {
                decodedInstr->carry = decodedInstr->operand2 <= registers[decodedInstr->Rn];
                processingUpdateCPSR(&registers[CPSR_REF],
                                     &registers[decodedInstr->Rd],
                                     &decodedInstr->carry);
            }
            return;
        case RSB:
            registers[decodedInstr->Rd] =
                    decodedInstr->operand2 - registers[decodedInstr->Rn];
            if (decodedInstr->S) {
                decodedInstr->carry = decodedInstr->operand2 >= registers[decodedInstr->Rn];
                processingUpdateCPSR(&registers[CPSR_REF],
                                     &registers[decodedInstr->Rd],
                                     &decodedInstr->carry);
            }
            return;
        case ADD:
            registers[decodedInstr->Rd] =
                    registers[decodedInstr->Rn] + decodedInstr->operand2;
            if (decodedInstr->S) {
                decodedInstr->carry = checkOverflow(&registers[decodedInstr->Rn],
                                                    &decodedInstr->operand2,
                                                    &registers[decodedInstr->Rd]);
                processingUpdateCPSR(&registers[CPSR_REF],
                                     &registers[decodedInstr->Rd],
                                     &decodedInstr->carry);
            }
            return;
        case TST:
            if (decodedInstr->S) {
                // We use decodedInstr->operand2 to store the value of the result, as its current value is
                // irrelevant for future processes
                decodedInstr->operand2 &= registers[decodedInstr->Rn];
                processingUpdateCPSR(&registers[CPSR_REF],
                                     &decodedInstr->operand2,
                                     &decodedInstr->carry);
            }
            return;
        case TEQ:
            if (decodedInstr->S) {
                // We use decodedInstr->operand2 to store the value of the result, as its current value is
                // irrelevant for future processes
                decodedInstr->operand2 ^= registers[decodedInstr->Rn];
                processingUpdateCPSR(&registers[CPSR_REF],
                                     &decodedInstr->operand2,
                                     &decodedInstr->carry);
            }
            return;
        case CMP:
            if (decodedInstr->S) {
                // We use decodedInstr->operand2 to store the value of the result, as its current value is
                // irrelevant for future processes
                decodedInstr->carry = decodedInstr->operand2 <= registers[decodedInstr->Rn];
                decodedInstr->operand2 = registers[decodedInstr->Rn] - decodedInstr->operand2;
                processingUpdateCPSR(&registers[CPSR_REF],
                                     &decodedInstr->operand2,
                                     &decodedInstr->carry);
            }
            return;
        case ORR:
            registers[decodedInstr->Rd] = decodedInstr->operand2 | registers[decodedInstr->Rn];
            if (decodedInstr->S) {
                processingUpdateCPSR(&registers[CPSR_REF],
                                     &registers[decodedInstr->Rd],
                                     &decodedInstr->carry);
            }
            return;
        case MOV:
            registers[decodedInstr->Rd] = decodedInstr->operand2;
            if (decodedInstr->S) {
                processingUpdateCPSR(&registers[CPSR_REF],
                                     &registers[decodedInstr->Rd],
                                     &decodedInstr->carry);
            }
            return;
        default:
            return;
    }
}

void executeMultiplying(uint32_t *registers, DecodedInstruction const *decodedInstr) {
	if (decodedInstr->A) {
		registers[decodedInstr->Rd] =
            registers[decodedInstr->Rm] * registers[decodedInstr->Rs] +
            registers[decodedInstr->Rn];
	} else {
		registers[decodedInstr->Rd] = registers[decodedInstr->Rm] * registers[decodedInstr->Rs];
	}
    if (decodedInstr->S) {
        multiplyingUpdateCPSR(&registers[CPSR_REF], &registers[decodedInstr->Rd]);
    }
}

void executeTransferring(uint32_t *registers, uint8_t *memory, DecodedInstruction *decodedInstr) {
    if (!(decodedInstr->U)) {
        decodedInstr->transferOffset = ~decodedInstr->transferOffset + 1;
    }
    uint32_t address = registers[decodedInstr->Rn];
    if (decodedInstr->P) {
        address += decodedInstr->transferOffset;
    } else {
        registers[decodedInstr->Rn] += decodedInstr->transferOffset;
    }
    // If the address is too big we return an error and leave the function
    if (address + 3 > MEMORY_SIZE) {
        printf("Error: Out of bounds memory access at address 0x%08x\n", address);
        return;
    }
    if (decodedInstr->L) {
        // Load
        registers[decodedInstr->Rd] = (((((memory[address + 3] << 8) |
                                          memory[address + 2]) << 8) |
                                        memory[address + 1]) << 8) |
                                      memory[address];
    } else {
        // Store
        memory[address] = registers[decodedInstr->Rd] & 0xff;
        memory[address + 1] = (registers[decodedInstr->Rd] >> 8) & 0xff;
        memory[address + 2] = (registers[decodedInstr->Rd] >> 16) & 0xff;
        memory[address + 3] = (registers[decodedInstr->Rd] >> 24) & 0xff;
    }
}

void executeBranch(uint32_t *registerPC, DecodedInstruction const *decodedInstr) {
    // We shift left by 8 and then arithmetically right by 6 which is equivalent to shifting 2 to
    // the left and then sign extending
    *registerPC += (decodedInstr->branchOffset << 8) >> 6;
}