#include <stdio.h>
#include <stdlib.h>

#define MEMORY_SIZE 65536
#define REGISTERS 17


int registers[REGISTERS];
int memory[MEMORY_SIZE];

int main(int argc, char **argv) {

    // State of ARM machine when turned on [Pg. 3 Spec]
    for (int i = 0; i < REGISTERS; i++) {
        registers[i] = 0;
    }
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }

    // TODO: LOAD BINARY FILE INTO MEMORY


    // pc is the program counter being regular incremented
    // at the end of the 3 stage pipeline which is essentially
    // the body of the loop
    // We know that instructions only occur at 4 byte boundaries
    // This means we can use a step of 4
    // The downfall of this however, is that a branching
    // address which is not a multiple of 4 would result
    // in many compounded errors due to pc mod 4 never being 0
    for (int pc = 0; pc < 65536; pc += 4) {
        // TODO: FETCH INSTRUCTION

        // TODO: DECODE INSTRUCTION

        // TODO: EXECUTE INSTRUCTION
    }

    return EXIT_SUCCESS;
}