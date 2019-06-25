#include "parser.h"

byte parse_register(char *token) {
    char *save;
    char *op = strtok_r(token, "r]", &save);
    return strtol(op, NULL, 10);
}

int parse_expression(char *token) {
    char *save;
    char *op = strtok_r(token, "r#=", &save);
    if (strstr(token, "x") != NULL) { // Hex
        return strtol(op, NULL, 16);
    } else { // Number or Register
        return strtol(op, NULL, 10);
    }
}

unsigned int parse_data_processing(char *save, Parser_Data *data) {
    byte opcode, Rn, Rd, I, S;
    unsigned int operand;

    opcode = get_address(data->mnemonic_table, data->mnemonic) & GET_FIRST_4_BITS;

    char *token;

    if (TST <= opcode && opcode <= CMP) { // tst, teq, cmp
        Rd = 0;
        S = 1; // S is always 1 if we have tst, teq, cmp
    } else {
        token = strtok_r(NULL, ", ", &save);
        Rd = parse_register(token);
        S = 0; // S is always 0 if we do not have tst, teq, cmp
    }

    if (opcode == MOV) { // mov
        Rn = 0; // Rn is irrelevant for mov instructions and should be set to 0
    } else {
        token = strtok_r(NULL, ", ", &save);
        Rn = parse_register(token);
    }

    token = strtok_r(NULL, ", \n", &save);
    I = (token[0] == '#');
    operand = parse_expression(token);

    if (I == 1) {
        // rotate operand if bigger than 8 bits
        if (operand > 0xff) {
            byte rotate = 0;
            while (operand > 0xff) {
                operand = (operand << 2) | (operand >> 30);
                rotate++;
                if (rotate > 0xf) {
                    perror("Cannot be represented");
                    break;
                }
            }
            operand = (rotate << 8) | (operand & GET_FIRST_8_BITS);
        }
    } else {
        // Check for shift input
        token = strtok_r(NULL, ", ", &save);
        if (token != NULL) {
            byte shift_type;
            unsigned int shift_operand;
            int imm = 0;

            shift_type = get_address(data->mnemonic_table, token) & GET_FIRST_4_BITS;
            token = strtok_r(NULL, " \n", &save);
            imm = (token[0] == '#');
            shift_operand = parse_expression(token);

            if (imm) {
                operand = ((shift_operand << 7) | (shift_type << 5) | operand) & GET_FIRST_12_BITS;
            } else {
                operand = ((shift_operand << 8) | (shift_type << 5) | (1 << 4) | operand) & GET_FIRST_12_BITS;
            }
        }
    }

    // The condition should always be AL in Data Processing
    return (AL << 28) | (I << 25) | (opcode << 21) | (S << 20) | (Rn << 16) | (Rd << 12) | operand;
}

unsigned int parse_multiply(char *save, Parser_Data *data) {
    byte A, Rd, Rn, Rs, Rm;

    char *token;

    token = strtok_r(NULL, ", ", &save);
    Rd = parse_register(token);
    token = strtok_r(NULL, ", ", &save);
    Rm = parse_register(token);
    token = strtok_r(NULL, ", \n", &save);
    Rs = parse_register(token);

    if (!strcmp(data->mnemonic, "mla")) { // mla
        token = strtok_r(NULL, " \n", &save);
        Rn = parse_register(token);
        A = 1;
    } else {
        Rn = 0; // Rn is irrelevant for mov instructions and should be set to 0
        A = 0;
    }

    // The condition should always be AL in Multiplying and S should be set to 0 (we are not considering it so it would
    // automatically be set to 0 which is what we want). Also 0x9 << 4 represents bits 7-4 which should be 1,0,0,1
    // respectively.
    return (AL << 28) | (A << 21) | (Rd << 16) | (Rn << 12) | (Rs << 8) | (0x9 << 4) | Rm;
}

unsigned int parse_data_transfer(char *save, Parser_Data *data) {
    byte I, P, U, L, Rn, Rd;
    unsigned short offset;

    L = (data->mnemonic[0] == 'l');

    char *token;

    token = strtok_r(NULL, ", ", &save);
    Rd = parse_register(token);

    token = strtok_r(NULL, "[, ]\n", &save);
    U = 1;
    if (token[0] == '=' && L) {
        unsigned int expression = parse_expression(token);
        if (expression > 0xff) {
            data->last_instr = data->last_instr + 1;
            int address = data->last_instr * 4;
            write_4byte_to_memory(data->memory, &expression, address);
            P = 1;
            I = 0;
            Rn = PC_REF; // PC
            offset = (data->last_instr - data->curr_instr) * 4 - 8;
        } else {
            // I should be set to 1 for this type of instructions
            return special_to_move_instruction(Rd, 1, expression, 0);
        }
    } else {
        I = 0;
        P = data->pre_indexed;
        Rn = parse_register(token);
        token = strtok_r(NULL, " ,]\n", &save);
        offset = 0;
        if (token != NULL) {
            I = (token[0] != '#');
            int expression = parse_expression(token);
            offset = abs(expression) & GET_FIRST_16_BITS;
            if (expression < 0) {
                U = 0;
            }
            token = strtok_r(NULL, " ,]\n", &save);
            if (token != NULL) {
                I = 1;
                byte shift_type;
                unsigned int shift_operand;
                int imm;

                shift_type = get_address(data->mnemonic_table, token) & GET_FIRST_4_BITS;
                token = strtok_r(NULL, " ,]\n", &save);
                imm = (token[0] == '#');
                shift_operand = parse_expression(token);
                if (imm) {
                    offset = ((shift_operand << 7) | (shift_type << 5) | offset) & GET_FIRST_12_BITS;
                } else {
                    offset = ((shift_operand << 8) | (shift_type << 5) | (1 << 4) | offset) & GET_FIRST_12_BITS;
                }
            }
        }
    }

    // The condition should always be AL in Multiplying
    return (AL << 28) | (1 << 26) | (I << 25) | (P << 24) | (U << 23) | (L << 20) | (Rn << 16) | (Rd << 12) | offset;
}

unsigned int parse_branch(char *save, Parser_Data *data) {
    char *token;
    byte cond;
    int offset;

    cond = get_address(data->mnemonic_table, data->mnemonic) & GET_FIRST_4_BITS;
    token = strtok_r(NULL, " \n", &save);
    int instr = data->curr_instr * 4;
    offset = (get_address(data->label_table, token) - instr - 8) >> 2;

    return (cond << 28) | (0xa << 24) | (offset & GET_FIRST_24_BITS);
}

unsigned int parse_special(char *save, Parser_Data *data) {
    // andeq
    if (!strcmp(data->mnemonic, "andeq")) {
        return 0;
    }

    // lsl
    char *token;
    byte Rn;
    unsigned short operand;

    token = strtok_r(NULL, ", ", &save);
    Rn = parse_register(token);

    token = strtok_r(NULL, ", ", &save);
    operand = parse_expression(token);

    // I should be set to 0 for lsl instructions
    return special_to_move_instruction(Rn, 0, Rn, operand);
}
