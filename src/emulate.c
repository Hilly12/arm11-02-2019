#include "pipeline.h"

int main(int argc, char **argv) {

    uint32_t registers[REGISTERS];

    uint8_t memory[MEMORY_SIZE];

    // State of ARM machine when turned on [Pg. 3 Spec]
    for (uint8_t i = 0; i < REGISTERS; i++) {
        registers[i] = 0;
    }
    for (uint32_t i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }

    // LOAD BINARY FILE INTO MEMORY
    FILE *binary = fopen(argv[1], "r");
    fread(memory, sizeof(uint8_t), MEMORY_SIZE, binary);
    fclose(binary);

    uint8_t fetchedInstr[8]; // Cond | 4 | 4 | r1 | r2 | 4 | 4 | 4

    DecodedInstruction decodedInstr;

    // First iteration (only fetching)
    fetch(&registers[PC_REF], memory, fetchedInstr);
    registers[PC_REF] += 4;

    // Second iteration (decoding and fetching);
    decode(fetchedInstr, registers, &decodedInstr);
    fetch(&registers[PC_REF], memory, fetchedInstr);
    registers[PC_REF] += 4;

    // Third iteration onward (executing, decoding and fetching)
    while (1) {
        // Execute instruction
        execute(registers, memory, &decodedInstr);

        // Break the loop if the last executed instruction was a HALT
        if (decodedInstr.type == ANDEQ) {
            break;
        }

        // Decode instruction if last instruction was not a branch
        if (decodedInstr.type == BRANCH) {
            decodedInstr.type = INVALID;
        } else {
            decode(fetchedInstr, registers, &decodedInstr);
        }

        // Fetch instruction if there are more instructions to consider
        if (registers[PC_REF] < MEMORY_SIZE) {
            fetch(&registers[PC_REF], memory, fetchedInstr);
            registers[PC_REF] += 4;
        } else {
            // Otherwise fetch an ANDEQ instruction
            for (uint8_t i = 0; i < 8; i++) {
                fetchedInstr[i] = 0;
            }
        }
    }

    output(registers, memory);

    return 0;
}
