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

void executeTransferring(uint32_t *registers, uint8_t *memory, uint8_t *gpio, uint8_t *gpio_on,
        uint8_t *gpio_off, DecodedInstruction *decodedInstr) {
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
        if (address >= GPIO_0_9_FIRST_BYTE && address <= GPIO_20_29_LAST_BYTE) {
            if (decodedInstr->L) {
                // If we are loading one of the 3 GPIO addresses, print a relevant message and set the value of the
                // register to the address
                gpioAccessPrint(&address);
                registers[decodedInstr->Rd] = address;
            } else {
                if (address > GPIO_20_29_FIRST_BYTE) {
                    // If the address is bigger than the address of the first byte of the 3rd GPIO address then return
                    // an error because we would get a out of bound access at the 4-th byte
                    printf("Error: Out of bounds memory access at address 0x%08x\n", address);
                } else {
                    // Else store as normal but to the specific GPIO memory addresses
                    gpioAccessPrint(&address);
                    address = address - GPIO_0_9_FIRST_BYTE;
                    store(&address, gpio, &registers[decodedInstr->Rd]);
                }
            }
        } else if (address == GPIO_ON_FIRST_BYTE) {
            // Accessing the address that turns the GPIO on
            address = 0;
            if (decodedInstr->L) {
                load(&address, gpio_on, &registers[decodedInstr->Rd]);
            } else {
                printf("PIN ON\n");
                store(&address, gpio_on, &registers[decodedInstr->Rd]);
            }
        } else if (address == GPIO_OFF_FIRST_BYTE) {
            // Accessing the address that turns the GPIO off
            address = 0;
            if (decodedInstr->L) {
                load(&address, gpio_off, &registers[decodedInstr->Rd]);
            } else {
                printf("PIN OFF\n");
                store(&address, gpio_off, &registers[decodedInstr->Rd]);
            }
        } else {
            printf("Error: Out of bounds memory access at address 0x%08x\n", address);
        }
        return;
    }
    if (decodedInstr->L) {
        load(&address, memory, &registers[decodedInstr->Rd]);
    } else {
        // Store
        store(&address, memory, &registers[decodedInstr->Rd]);
    }
}

void executeBranch(uint32_t *registerPC, DecodedInstruction const *decodedInstr) {
    // We shift left by 8 and then arithmetically right by 6 which is equivalent to shifting 2 to
    // the left and then sign extending
    *registerPC += (decodedInstr->branchOffset << 8) >> 6;
}
