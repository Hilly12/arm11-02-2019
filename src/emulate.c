#include "pipeline.h"

int main(int argc, char **argv) {

    if (argc != 2) {
        perror("Wrong number of arguments");
        return 1;
    }

    unsigned int registers[REGISTERS];

    byte memory[MEMORY_SIZE];

    // State of ARM machine when turned on
    for (byte i = 0; i < REGISTERS; i++) {
        registers[i] = 0;
    }
    for (unsigned int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }

    // Stores the addresses related to accessing GPIO pins and initializes them to 0
    byte gpio[GPIOBYTES];
    for (byte i = 0; i < GPIOBYTES; i++) {
        gpio[i] = 0;
    }

    // Stores the addresses that turn the GPIO on and off and initializes them to 0
    byte gpio_on[GPIO_ON_OFF_BYTES];
    byte gpio_off[GPIO_ON_OFF_BYTES];
    for (byte i = 0; i < GPIO_ON_OFF_BYTES; i++) {
        gpio_on[i] = 0;
        gpio_off[i] = 0;
    }

    // LOAD BINARY FILE INTO MEMORY
    FILE *binary = fopen(argv[1], "r");
    fread(memory, sizeof(byte), MEMORY_SIZE, binary);
    fclose(binary);

    byte fetched_instr[8]; // Cond | 4 | 4 | r1 | r2 | 4 | 4 | 4

    Decoded_Instruction decoded_instr;

    // First iteration (only fetching)
    fetch(&registers[PC_REF], memory, fetched_instr);
    registers[PC_REF] += 4;

    // Second iteration (decoding and fetching);
    decode(fetched_instr, registers, &decoded_instr);
    fetch(&registers[PC_REF], memory, fetched_instr);
    registers[PC_REF] += 4;

    // Third iteration onward (executing, decoding and fetching)
    while (1) {
        // Execute instruction
        execute(registers, memory, gpio, gpio_on, gpio_off, &decoded_instr);

        // Break the loop if the last executed instruction was a HALT
        if (decoded_instr.type == ANDEQ) {
            break;
        }

        // Decode instruction if last instruction was not a branch
        if (decoded_instr.type == BRANCH) {
            decoded_instr.type = INVALID;
        } else {
            decode(fetched_instr, registers, &decoded_instr);
        }

        // Fetch instruction if there are more instructions to consider
        if (registers[PC_REF] < MEMORY_SIZE) {
            fetch(&registers[PC_REF], memory, fetched_instr);
            registers[PC_REF] += 4;
        } else {
            // Otherwise fetch an ANDEQ instruction
            for (byte i = BITS_28_TO_31; i <= BITS_28_TO_31; i++) {
                fetched_instr[i] = 0;
            }
        }
    }

    output(registers, memory);

    return 0;
}
