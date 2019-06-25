#include "assemble_utils.h"

int special_to_move_instruction(byte Rd, byte I, byte operand, byte shift_operand) {
    // The condition should always be AL in Data Processing
    return (AL << 28) | (I << 25) | (MOV << 21) | (Rd << 12) | (((shift_operand << 7) | operand) & GET_FIRST_12_BITS);
}

void write_4byte_to_memory(byte *memory, unsigned int const *instruction, int address) {
    unsigned int instr_buffer = *instruction;
    for (byte j = 0; j < 4; j++) {
        memory[address + j] = instr_buffer & GET_FIRST_8_BITS;
        instr_buffer = instr_buffer >> 8;
    }
}

void save_to_file(char const *file_name, byte const *memory, unsigned int const *last_instr) {
    FILE *file;
    file = fopen(file_name, "wb");
    fwrite(memory, (*last_instr + 1) * 4 * sizeof(byte), 1, file);
    fclose(file);
}
