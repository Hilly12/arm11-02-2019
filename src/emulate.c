#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define MEMORY_SIZE 65536
#define REGISTERS 17
#define PC 15
#define CPSR 16

enum instructionType {processing, multiplying, transfer, branch, halt, invalid};

int registers[REGISTERS];
unsigned char memory[MEMORY_SIZE];
int finished = 0;

// Found on the internet
void printHexadec(int n) {
    char hexadec[9];
    sprintf(hexadec, "%x", n);
    printf("0x");
    for (int i = strlen(hexadec); i < 8; i++) {
        printf("0");
    }
    printf("%s", hexadec);
}

int concat4bytes(int fst, int snd, int trd, int frth) {
    return (((fst << 8) | snd) << 16) | ((trd << 8) | frth);
}

// Returns the k-th bit
int getBit(int n, int k) {
    return (n >> k) & 1;
}

// Found on the Internet
// Returns n with k-th bit changed to x (x could be either 0 or 1)
int changeBit (int n, int k, int x) {
    return n ^ ((-x ^ n) & (1UL << k));
}

// I wrote this
int logicalLeft(int n, int k) {
    return n << k;
}

// Found on the Internet
int logicalRight(int n, int k) {
    return (int)((unsigned int)n >> k);
}

// I wrote this
int arithmeticRight(int n, int k) {
    return n >> k;
}

// Found on the Internet
int rotateRight(int n, int k) {
    return n >> k | n << (32-k);
}

bool checkOverflow(int a, int b) {
    int result = a + b;
    if(a > 0 && b > 0 && result < 0)
        return true;
    if(a < 0 && b < 0 && result > 0)
        return true;
    return false;
}

// I wrote this
// Returns the bit range of length k starting from bit n (left to right, from higher to lower)
int getBitRange(int binary, int n, int k) {
    return logicalRight((binary << (31 - n)), 32 - k);
}

bool isConditionValid(int binary) {
    switch(binary) {
        case 0: // eq
        {
            return getBit(registers[CPSR], 30) == 1;
        }
        case 1: // ne
        {
            return getBit(registers[CPSR], 30) == 0;
        }
        case 10: // ge
        {
            return getBit(registers[CPSR], 31) == getBit(registers[CPSR], 28);
        }
        case 11: // lt
        {
            return getBit(registers[CPSR], 31) != getBit(registers[CPSR], 28);
        }
        case 12: // gt
        {
            return getBit(registers[CPSR], 30) == 0 && getBit(registers[CPSR], 31) == getBit(registers[CPSR], 28);
        }
        case 13: // le
        {
            return getBit(registers[CPSR], 30) == 1 || getBit(registers[CPSR], 31) != getBit(registers[CPSR], 28);
        }
        case 14: // al
        {
            return true;
        }
        default:
            printf("Error\n");
            return false;
    }
}

struct decodedCode{
    enum instructionType instructionType;
    int opcode; // For processing
    int S; // For processing, multiplying
    int Rn; // For processing, multiplying, transferring
    int Rd; // For processing, multiplying, transferring
    int Rm; // For processing, multiplying, transferring
    int Rs; // For multiplying
    int operand2; // For processing
    int carryOut; // For processing
    int A; // For multiplying
    int P; // For transferring
    int U; // For transferring
    int L; // For transferring
    int offset; // For transferring and branch
};

void decodeProcessing(int binary, struct decodedCode *decodedCode) {
    (*decodedCode).instructionType = processing;
    (*decodedCode).opcode = getBitRange(binary, 24, 4);
    (*decodedCode).S = getBit(binary, 20);
    (*decodedCode).Rn = getBitRange(binary, 19, 4);
    (*decodedCode).Rd = getBitRange(binary, 15, 4);
    if (getBit(binary, 25) == 1) {
        int rotate = getBitRange(binary, 11, 4);
        int imm = getBitRange(binary, 7, 8);
        (*decodedCode).carryOut = (imm == 0) ? 0 : getBit(imm, 2 * rotate - 1);
        (*decodedCode).operand2 = rotateRight(imm, 2 * rotate);
    } else {
        (*decodedCode).Rm = getBitRange(binary, 3, 4);
        int shiftType = getBitRange(binary, 6, 2);
        if (getBit(binary, 4) == 1 && getBit(binary, 7) == 0) {
            (*decodedCode).carryOut = 0;
            (*decodedCode).operand2 = registers[(*decodedCode).Rm];
        } else {
            int shiftValue = (getBit(binary, 4) == 0) ? getBitRange(binary, 11, 5) : registers[getBitRange(binary, 11,
                                                                                                           4)];
            switch (shiftType) {
                case 0: // logical left
                    (*decodedCode).carryOut = (shiftValue == 0) ? 0 : getBit(registers[(*decodedCode).Rm],
                                                                             31 - shiftValue);
                    (*decodedCode).operand2 = logicalLeft(registers[(*decodedCode).Rm], shiftValue);
                    break;
                case 1: // logical right
                    (*decodedCode).carryOut = (shiftValue == 0) ? 0 : getBit(registers[(*decodedCode).Rm],
                                                                             shiftValue - 1);
                    (*decodedCode).operand2 = logicalRight(registers[(*decodedCode).Rm], shiftValue);
                    break;
                case 2: // arithmetic right
                    (*decodedCode).carryOut = (shiftValue == 0) ? 0 : getBit(registers[(*decodedCode).Rm],
                                                                             shiftValue - 1);
                    (*decodedCode).operand2 = arithmeticRight(registers[(*decodedCode).Rm], shiftValue);
                    break;
                case 3: // rotate right
                    (*decodedCode).carryOut = (shiftValue == 0) ? 0 : getBit(registers[(*decodedCode).Rm],
                                                                             shiftValue - 1);
                    (*decodedCode).operand2 = rotateRight(registers[(*decodedCode).Rm], shiftValue);
                    break;
                default:
                    printf("Error\n");
            }
        }
    }
}

void decodeMultiplying(int binary, struct decodedCode *decodedCode) {
    (*decodedCode).instructionType = multiplying;
    (*decodedCode).A = getBit(binary, 21);
    (*decodedCode).S = getBit(binary, 20);
    (*decodedCode).Rd = getBitRange(binary, 19, 4);
    (*decodedCode).Rn = getBitRange(binary, 15, 4);
    (*decodedCode).Rs = getBitRange(binary, 11, 4);
    (*decodedCode).Rm = getBitRange(binary, 3, 4);
}

void decodeTransfer(int binary, struct decodedCode *decodedCode) {
    (*decodedCode).instructionType = transfer;
    (*decodedCode).P = getBit(binary, 24);
    (*decodedCode).U = getBit(binary, 23);
    (*decodedCode).L = getBit(binary, 20);
    (*decodedCode).Rn = getBitRange(binary, 19, 4);
    (*decodedCode).Rd = getBitRange(binary, 15, 4);
    if (getBit(binary, 25) == 0) {
        (*decodedCode).offset = getBitRange(binary, 11, 12);
    } else {
        (*decodedCode).Rm = getBitRange(binary, 3, 4);
        int shiftType = getBitRange(binary, 6, 2);
        if (getBit(binary, 4) == 1 && getBit(binary, 7) == 0) {
            (*decodedCode).offset = registers[(*decodedCode).Rm];
        } else {
            int shiftValue = (getBit(binary, 4) == 0) ? getBitRange(binary, 11, 5) : registers[getBitRange(binary, 11,
                                                                                                           4)];
            switch (shiftType) {
                case 0: // logical left
                    (*decodedCode).offset = logicalLeft(registers[(*decodedCode).Rm], shiftValue);
                    break;
                case 1: // logical right
                    (*decodedCode).offset = logicalRight(registers[(*decodedCode).Rm], shiftValue);
                    break;
                case 2: // arithmetic right
                    (*decodedCode).offset = arithmeticRight(registers[(*decodedCode).Rm], shiftValue);
                    break;
                case 3: // rotate right
                    (*decodedCode).offset = rotateRight(registers[(*decodedCode).Rm], shiftValue);
                    break;
                default:
                    printf("Error\n");
            }
        }
    }
}

void decodeBranch(int binary, struct decodedCode *decodedCode) {
    (*decodedCode).instructionType = branch;
    (*decodedCode).offset = arithmeticRight(logicalLeft(getBitRange(binary, 23, 24), 8), 6);
}

void decode(int binary, struct decodedCode *decodedCode) {
    if (binary == 0) {
        (*decodedCode).instructionType = halt;
    } else {
        int condInt = getBitRange(binary, 31, 4);
        if (isConditionValid(condInt)) {
            // If the 27-th bit is 1 we have a branch
            if (getBit(binary, 27) == 1) {
                decodeBranch(binary, decodedCode);
            }
                // If the 26-th bit is 1 we have a transfer;
            else if (getBit(binary, 26) == 1) {
                decodeTransfer(binary, decodedCode);
            }
                // Not 100% sure about the condition for Multiplying
            else if (getBit(binary, 7) == 1 && getBit(binary, 6) == 0 && getBit(binary, 5) == 0 &&
                     getBit(binary, 4) == 1) {
                decodeMultiplying(binary, decodedCode);
            } else {
                decodeProcessing(binary, decodedCode);
            }
        } else {
            (*decodedCode).instructionType = invalid;
        }
    }
}

void executeProcessing(struct decodedCode decodedCode) {
    int result;
    switch (decodedCode.opcode) {
        case 0: // and
            result = registers[decodedCode.Rn] & decodedCode.operand2;
            registers[decodedCode.Rd] = result;
            break;
        case 1: // eor
            result = registers[decodedCode.Rn] ^ decodedCode.operand2;
            registers[decodedCode.Rd] = result;
            break;
        case 2: // sub
            result = registers[decodedCode.Rn] - decodedCode.operand2;
            registers[decodedCode.Rd] = result;
            if (decodedCode.operand2 > decodedCode.Rn) {
                decodedCode.carryOut = 0;
            } else {
                decodedCode.carryOut = 1;
            }
            break;
        case 3: // rsb
            result = decodedCode.operand2 - registers[decodedCode.Rn];
            registers[decodedCode.Rd] = result;
            if (decodedCode.operand2 < decodedCode.Rn) {
                decodedCode.carryOut = 0;
            } else {
                decodedCode.carryOut = 1;
            }
            break;
        case 4: // add
            result = registers[decodedCode.Rn] + decodedCode.operand2;
            registers[decodedCode.Rd] = result;
            if (checkOverflow(registers[decodedCode.Rn], decodedCode.operand2)) {
                decodedCode.carryOut = 1;
            } else {
                decodedCode.carryOut = 0;
            }
            break;
        case 8: // tst
            result = registers[decodedCode.Rn] & decodedCode.operand2;
            break;
        case 9: // teq
            result = registers[decodedCode.Rn] ^ decodedCode.operand2;
            break;
        case 10: // cmp
            result = registers[decodedCode.Rn] - decodedCode.operand2;
            if (decodedCode.operand2 > registers[decodedCode.Rn]) {
                decodedCode.carryOut = 0;
            } else {
                decodedCode.carryOut = 1;
            }
            break;
        case 12: // orr
            result = registers[decodedCode.Rn] | decodedCode.operand2;
            registers[decodedCode.Rd] = result;
            break;
        case 13: // mov
            result = decodedCode.operand2;
            registers[decodedCode.Rd] = result;
            break;
        default:
            printf("Error\n");
    }
    if (decodedCode.S == 1) {
        registers[CPSR] = changeBit(registers[CPSR], 29, decodedCode.carryOut);
        if (result == 0) {
            registers[CPSR] = changeBit(registers[CPSR], 30, 1);
        } else {
            registers[CPSR] = changeBit(registers[CPSR], 30, 0);
        }
        registers[CPSR] = changeBit(registers[CPSR], 31, getBit(result, 31));
    }
}

void executeMultiplying(struct decodedCode decodedCode) {
    if (decodedCode.A == 0) {
        registers[decodedCode.Rd] = registers[decodedCode.Rm] * registers[decodedCode.Rs];
    } else {
        registers[decodedCode.Rd] = registers[decodedCode.Rm] * registers[decodedCode.Rs] + registers[decodedCode.Rn];
    }
    if (decodedCode.S == 1) {
        if (registers[decodedCode.Rd] == 0) {
            registers[CPSR] = changeBit(registers[CPSR], 30, 1);
        }
        registers[CPSR] = changeBit(registers[CPSR], 31, getBit(registers[decodedCode.Rd], 31));
    }
}

void executeTransfer(struct decodedCode decodedCode) {
    int result = (decodedCode.U == 1) ? (registers[decodedCode.Rn] + decodedCode.offset) : (registers[decodedCode.Rn] -
                                                                                         decodedCode.offset);
    if (decodedCode.L == 1) { // load
        if (decodedCode.P == 1) {
            registers[decodedCode.Rd] = memory[result];
        } else {
            registers[decodedCode.Rd] = memory[registers[decodedCode.Rn]];
            registers[decodedCode.Rn] = result;
        }
    } else {
        if (decodedCode.P == '1') { // store
            memory[result] = registers[decodedCode.Rd];
        } else {
            memory[registers[decodedCode.Rn]] = registers[decodedCode.Rd];
            registers[decodedCode.Rn] = result;
        }
    }
}

void executeBranch(struct decodedCode decodedCode) {
    registers[PC] = registers[PC] + decodedCode.offset;
}


void execute(struct decodedCode decodedCode) {
    switch (decodedCode.instructionType) {
        case halt:
        case invalid:
            break;
        case processing:
            executeProcessing(decodedCode);
            break;
        case multiplying:
            executeMultiplying(decodedCode);
            break;
        case transfer:
            executeTransfer(decodedCode);
            break;
        case branch:
            executeBranch(decodedCode);
            break;
        default:
            printf("Error\n");
    }
}

int main(int argc, char **argv) {

    // State of ARM machine when turned on [Pg. 3 Spec]
    for (int i = 0; i < REGISTERS; i++) {
        registers[i] = 0;
    }
    for (int i = 0; i < MEMORY_SIZE; i++) {
        memory[i] = 0;
    }

    //  LOAD BINARY FILE INTO MEMORY [Copy pasted Ruari's code and changed memory to an integer array instead of string]
    FILE *binary = fopen(argv[1], "r"); //open file
    fread(memory, sizeof(unsigned char), MEMORY_SIZE, binary);
    fclose(binary); //close file

    // Pipeline
    // Changed pc to register[PC] because as far as I understand we should increment the register after
    // every iteration. Correct me if I am wrong.
    // First cycle: Fetch
    int lastFetch;
    lastFetch = concat4bytes(memory[registers[PC] + 3], memory[registers[PC] + 2], memory[registers[PC] + 1],
                             memory[registers[PC]]);
    registers[PC] += 4;
    // Second cycle: Decode -> Fetch
    struct decodedCode lastDecode;
    decode(lastFetch, &lastDecode);
    lastFetch = concat4bytes(memory[registers[PC] + 3], memory[registers[PC] + 2], memory[registers[PC] + 1],
                             memory[registers[PC]]);
    registers[PC] += 4;
    // Third cycle and onward: Execute -> Decode -> Fetch
    for (; registers[PC] < 65536; registers[PC] += 4) {
        // Execute:
        execute(lastDecode);

        // Decode:
        // Note that rather than storing the information to a new struct decodeCode, we rewrite the old one which
        // would mean that if some code is unchanged from the previous iteration, it would remain (for example the
        // decodedCode for a branch instruction may contain an opcode field). That however is irrelevant because
        // if our execute function works properly, the irrelevant fields would not be used in any way.
        // If the executed function was a branch then the next decoded function is invalid thus we have the if statement
        if (lastDecode.instructionType == halt) {
            break;
        } else {
            if (lastDecode.instructionType == branch) {
                lastDecode.instructionType = invalid;
            } else {
                decode(lastFetch, &lastDecode);
            }
        }

        // Fetch:
        lastFetch = concat4bytes(memory[registers[PC] + 3], memory[registers[PC] + 2], memory[registers[PC] + 1],
                                 memory[registers[PC]]);

    }

    //Output
    printf("Registers:\n");
    printf("$0  :");
    if (registers[0] < -1000000000) {
        printf(" ");
    }
    printf("%*d", 11, registers[0]);
    printf(" (");
    printHexadec(registers[0]);
    printf(")\n");
    printf("$1  :");
    if (registers[1] < -1000000000) {
        printf(" ");
    }
    printf("%*d", 11, registers[1]);
    printf(" (");
    printHexadec(registers[1]);
    printf(")\n");
    printf("$2  :");
    if (registers[2] < -1000000000) {
        printf(" ");
    }
    printf("%*d", 11, registers[2]);
    printf(" (");
    printHexadec(registers[2]);
    printf(")\n");
    printf("$3  :");
    if (registers[3] < -1000000000) {
        printf(" ");
    }
    printf("%*d", 11, registers[3]);
    printf(" (");
    printHexadec(registers[3]);
    printf(")\n");
    printf("$4  :");
    if (registers[4] < -1000000000) {
        printf(" ");
    }
    printf("%*d", 11, registers[4]);
    printf(" (");
    printHexadec(registers[4]);
    printf(")\n");
    printf("$5  :");
    if (registers[5] < -1000000000) {
        printf(" ");
    }
    printf("%*d", 11, registers[5]);
    printf(" (");
    printHexadec(registers[5]);
    printf(")\n");
    printf("$6  :");
    if (registers[6] < -1000000000) {
        printf(" ");
    }
    printf("%*d", 11, registers[6]);
    printf(" (");
    printHexadec(registers[6]);
    printf(")\n");
    printf("$7  :");
    if (registers[7] < -1000000000) {
        printf(" ");
    }
    printf("%*d", 11, registers[7]);
    printf(" (");
    printHexadec(registers[7]);
    printf(")\n");
    printf("$8  :");
    if (registers[8] < -1000000000) {
        printf(" ");
    }
    printf("%*d", 11, registers[8]);
    printf(" (");
    printHexadec(registers[8]);
    printf(")\n");
    printf("$9  :");
    if (registers[9] < -1000000000) {
        printf(" ");
    }
    printf("%*d", 11, registers[9]);
    printf(" (");
    printHexadec(registers[9]);
    printf(")\n");
    printf("$10 :");
    if (registers[10] < -1000000000) {
        printf(" ");
    }
    printf("%*d", 11, registers[10]);
    printf(" (");
    printHexadec(registers[10]);
    printf(")\n");
    printf("$11 :");
    if (registers[11] < -1000000000) {
        printf(" ");
    }
    printf("%*d", 11, registers[11]);
    printf(" (");
    printHexadec(registers[11]);
    printf(")\n");
    printf("$12 :");
    if (registers[12] < -1000000000) {
        printf(" ");
    }
    printf("%*d", 11, registers[12]);
    printf(" (");
    printHexadec(registers[12]);
    printf(")\n");
    printf("PC  :");
    if (registers[PC] < -1000000000) {
        printf(" ");
    }
    printf("%*d", 11, registers[PC]);
    printf(" (");
    printHexadec(registers[PC]);
    printf(")\n");
    printf("CPSR:");
    if (registers[CPSR] < -1000000000) {
        printf(" ");
    }
    printf("%*d", 11, registers[CPSR]);
    printf(" (");
    printHexadec(registers[CPSR]);
    printf(")\n");
    printf("Non-zero memory:\n");
    for (int j = 0; j < registers[PC] - 8; j += 4) {
        printHexadec(j);
        printf(": ");
        printHexadec(concat4bytes(memory[j], memory[j + 1], memory[j + 2], memory[j + 3]));
        printf("\n");
    }

    return EXIT_SUCCESS;
}