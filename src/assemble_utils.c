#include "assemble_utils.h"

int to_move_instruction(byte Rd, byte I, byte operand, byte shift_type, byte shift_operand) {
    return (0xe << 28) | (I << 25) | (0xd << 21) | (Rd << 12)
           | (((shift_operand << 7) | (shift_type << 5) | operand) & 0xfff);
}

void write_4byte_to_memory(byte *memory, unsigned int const *instruction, int const *address) {
    unsigned int instr_buffer = *instruction;
    for (byte j = 0; j < 4; j++) {
        memory[*address + j] = instr_buffer & 0xff;
        instr_buffer = instr_buffer >> 8;
    }
}

void save_to_file(char const *file_name, byte const *memory, unsigned int const *last_instr) {
    FILE *file;
    file = fopen(file_name, "wb");
    fwrite(memory, (*last_instr + 1) * 4 * sizeof(byte), 1, file);
    fclose(file);
}
