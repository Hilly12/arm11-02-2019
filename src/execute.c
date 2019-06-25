#include "execute.h"

void execute_processing(unsigned int *registers, Decoded_Instruction *decoded_instr) {
    switch (decoded_instr->opcode) {
        case AND:
            registers[decoded_instr->Rd] = registers[decoded_instr->Rn] & decoded_instr->operand2;
            if (decoded_instr->S) {
                processing_update_CPSR(&registers[CPSR_REF],
                                       &registers[decoded_instr->Rd],
                                       &decoded_instr->carry);
            }
            return;
        case EOR:
            registers[decoded_instr->Rd] = registers[decoded_instr->Rn] ^ decoded_instr->operand2;
            if (decoded_instr->S) {
                processing_update_CPSR(&registers[CPSR_REF],
                                       &registers[decoded_instr->Rd],
                                       &decoded_instr->carry);
            }
            return;
        case SUB:
            registers[decoded_instr->Rd] = registers[decoded_instr->Rn] - decoded_instr->operand2;
            if (decoded_instr->S) {
                decoded_instr->carry = decoded_instr->operand2 <= registers[decoded_instr->Rn];
                processing_update_CPSR(&registers[CPSR_REF],
                                       &registers[decoded_instr->Rd],
                                       &decoded_instr->carry);
            }
            return;
        case RSB:
            registers[decoded_instr->Rd] = decoded_instr->operand2 - registers[decoded_instr->Rn];
            if (decoded_instr->S) {
                decoded_instr->carry = decoded_instr->operand2 >= registers[decoded_instr->Rn];
                processing_update_CPSR(&registers[CPSR_REF],
                                       &registers[decoded_instr->Rd],
                                       &decoded_instr->carry);
            }
            return;
        case ADD:
            registers[decoded_instr->Rd] = registers[decoded_instr->Rn] + decoded_instr->operand2;
            if (decoded_instr->S) {
                decoded_instr->carry = check_overflow(&registers[decoded_instr->Rn],
                                                      &decoded_instr->operand2,
                                                      &registers[decoded_instr->Rd]);
                processing_update_CPSR(&registers[CPSR_REF],
                                       &registers[decoded_instr->Rd],
                                       &decoded_instr->carry);
            }
            return;
        case TST:
            if (decoded_instr->S) {
                // We use decoded_instr->operand2 to store the value of the result, as its current value is
                // irrelevant for future processes
                decoded_instr->operand2 &= registers[decoded_instr->Rn];
                processing_update_CPSR(&registers[CPSR_REF],
                                       &decoded_instr->operand2,
                                       &decoded_instr->carry);
            }
            return;
        case TEQ:
            if (decoded_instr->S) {
                // We use decoded_instr->operand2 to store the value of the result, as its current value is
                // irrelevant for future processes
                decoded_instr->operand2 ^= registers[decoded_instr->Rn];
                processing_update_CPSR(&registers[CPSR_REF],
                                       &decoded_instr->operand2,
                                       &decoded_instr->carry);
            }
            return;
        case CMP:
            if (decoded_instr->S) {
                // We use decoded_instr->operand2 to store the value of the result, as its current value is
                // irrelevant for future processes
                decoded_instr->carry = decoded_instr->operand2 <= registers[decoded_instr->Rn];
                decoded_instr->operand2 = registers[decoded_instr->Rn] - decoded_instr->operand2;
                processing_update_CPSR(&registers[CPSR_REF],
                                       &decoded_instr->operand2,
                                       &decoded_instr->carry);
            }
            return;
        case ORR:
            registers[decoded_instr->Rd] = decoded_instr->operand2 | registers[decoded_instr->Rn];
            if (decoded_instr->S) {
                processing_update_CPSR(&registers[CPSR_REF],
                                       &registers[decoded_instr->Rd],
                                       &decoded_instr->carry);
            }
            return;
        case MOV:
            registers[decoded_instr->Rd] = decoded_instr->operand2;
            if (decoded_instr->S) {
                processing_update_CPSR(&registers[CPSR_REF],
                                       &registers[decoded_instr->Rd],
                                       &decoded_instr->carry);
            }
            return;
        default:
            return;
    }
}

void execute_multiplying(unsigned int *registers, Decoded_Instruction const *decoded_instr) {
    if (decoded_instr->A) {
        registers[decoded_instr->Rd] =
                registers[decoded_instr->Rm] * registers[decoded_instr->Rs] + registers[decoded_instr->Rn];
    } else {
        registers[decoded_instr->Rd] = registers[decoded_instr->Rm] * registers[decoded_instr->Rs];
    }
    if (decoded_instr->S) {
        multiplying_update_CPSR(&registers[CPSR_REF], &registers[decoded_instr->Rd]);
    }
}

void execute_transferring(unsigned int *registers, byte *memory, byte *gpio, byte *gpio_on,
                          byte *gpio_off, Decoded_Instruction *decoded_instr) {
    if (!(decoded_instr->U)) {
        decoded_instr->transfer_offset = ~decoded_instr->transfer_offset + 1;
    }
    unsigned int address = registers[decoded_instr->Rn];
    if (decoded_instr->P) {
        address += decoded_instr->transfer_offset;
    } else {
        registers[decoded_instr->Rn] += decoded_instr->transfer_offset;
    }

    // If the address is too big we check if that address is a GPIO
    if (address + 3 > MEMORY_SIZE) {
        // Checking if the address is one of the 3 GPIO addresses
        if (address >= GPIO_0_9_FIRST_BYTE && address <= GPIO_20_29_LAST_BYTE) {
            if (decoded_instr->L) {
                // If we are loading one of the 3 GPIO addresses, print a relevant message and set the value of the
                // register to the address
                gpio_access_print(&address);
                registers[decoded_instr->Rd] = address;
            } else {
                // Storing
                if (address > GPIO_20_29_FIRST_BYTE) {
                    // If the address is bigger than the address of the first byte of the 3rd GPIO address then return
                    // an error because we would get a out of bound access at the 4-th byte
                    printf("Error: Out of bounds memory access at address 0x%08x\n", address);
                } else {
                    // Else store as normal to the specific GPIO memory addresses
                    gpio_access_print(&address);
                    // Address becomes address - GPIO_0_9_FIRST_BYTE because we are using a new separate array to store
                    // the values of the memory addresses that are related to accessing the GPIO pins
                    address = address - GPIO_0_9_FIRST_BYTE;
                    store(&address, gpio, &registers[decoded_instr->Rd]);
                }
            }
        } else if (address == GPIO_ON_FIRST_BYTE) { // Checks if we are accessing the address that turns the GPIO on
            // Address is 0 because we are using a new separate array to store the values of the memory addresses that
            // are related to turning on the GPIO
            address = 0;
            // Loading and storing as normal (storing prints a relevant message)
            if (decoded_instr->L) {
                load(&address, gpio_on, &registers[decoded_instr->Rd]);
            } else {
                printf("PIN ON\n");
                store(&address, gpio_on, &registers[decoded_instr->Rd]);
            }
        } else if (address == GPIO_OFF_FIRST_BYTE) { // Checks if we are accessing the address that turns the GPIO off
            // Address is 0 because we are using a new separate array to store the values of the memory addresses that
            // are related to turning off the GPIO
            address = 0;
            // Loading and storing as normal (storing prints a relevant message)
            if (decoded_instr->L) {
                load(&address, gpio_off, &registers[decoded_instr->Rd]);
            } else {
                printf("PIN OFF\n");
                store(&address, gpio_off, &registers[decoded_instr->Rd]);
            }
        } else {
            // If the address is too big and not a GPIO related then we print an error
            printf("Error: Out of bounds memory access at address 0x%08x\n", address);
        }
        return;
    }

    if (decoded_instr->L) {
        load(&address, memory, &registers[decoded_instr->Rd]);
    } else {
        store(&address, memory, &registers[decoded_instr->Rd]);
    }
}

void execute_branch(unsigned int *pc_register, Decoded_Instruction const *decoded_instr) {
    // We shift left by 8 and then arithmetically right by 6 which is equivalent to shifting 2 to
    // the left and then sign extending
    *pc_register += (decoded_instr->branch_offset << 8) >> 6;
}
