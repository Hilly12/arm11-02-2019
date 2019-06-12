#include "assemble_utils.h"

int to_move_instruction(uint8_t Rd, uint8_t I, uint8_t operand, uint8_t shift_type, uint8_t shift_operand) {
    return (0xe << 28) | (I << 25) | (0xd << 21) | (Rd << 12)
           | (((shift_operand << 7) | (shift_type << 5) | operand) & 0xfff);
}

int generate_branch_offset(Symbol_Table *sym_table, char *label, int cuur_instr) {
    return (((int) get_address(sym_table, label)) - cuur_instr - 8) >> 2;
}

void write_4byte_to_memory(uint8_t *memory, int instruction, int address) {
    int instr_buffer = instruction;
    for (int j = 0; j < 4; j++) {
        memory[address + j] = instr_buffer & 0xff;
        instr_buffer = instr_buffer >> 8;
    }
}

void save_to_file(char *file_name, uint8_t *memory, int last_instr) {
    FILE *file;
    file = fopen(file_name, "wb");
    fwrite(memory, (last_instr + 1) * 4 * sizeof(uint8_t), 1, file);
    fclose(file);
}
