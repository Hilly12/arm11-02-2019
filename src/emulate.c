#include "pipeline.h"

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
    decodedInstruction decodedInstr;
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

    output(registers, memory);

    return 0;
}
