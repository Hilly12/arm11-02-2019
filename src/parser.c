#include "parser.h"

uint8_t parse_register(char *token) {
    char *save;
    char *op = strtok_r(token, "r]", &save);
    return strtol(op, NULL, 10);;
}

int32_t parse_expression(char *token) {
    char *save;
    char *op = strtok_r(token, "r#=", &save);
    if (strstr(token, "x") != NULL) { // Hex
        return strtol(op, NULL, 16);
    } else { // Number or Register
        return strtol(op, NULL, 10);
    }
}

int parse_data_processing(char *save, Parser_Data *dat) {
    uint8_t cond, opcode, Rn, Rd, I, S;
    uint32_t operand;

    cond = 0xe;
    opcode = get_address(dat->opcode_table, dat->mnemonic) & 0xf;

    char *token;

    if (0x8 <= opcode && opcode <= 0xa) { // tst, teq, cmp
        Rd = 0;
        S = 1;
    } else {
        token = strtok_r(NULL, ", ", &save);
        Rd = parse_register(token);
        S = 0;
    }

    if (opcode == 0xd) { // mov
        Rn = 0;
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
            uint8_t rotate = 0;
            while (operand > 0xff) {
                operand = (operand << 2) | (operand >> 30);
                rotate++;
                if (rotate > 0xf) {
                    perror("Cannot be represented");
                    break;
                }
            }
            operand = (rotate << 8) | (operand & 0xff);
        }
    } else {
        // Check for shift input
        token = strtok_r(NULL, ", ", &save);
        if (token != NULL) {
            uint8_t shift_type;
            uint32_t shift_operand;
            int imm = 0;

            shift_type = get_address(dat->opcode_table, token) & 0xf;

            token = strtok_r(NULL, " \n", &save);
            imm = (token[0] == '#');
            shift_operand = parse_expression(token);

            if (imm) {
                operand = ((shift_operand << 7) | (shift_type << 5) | operand) & 0xfff;
            } else {
                operand = ((shift_operand << 8) | (shift_type << 5) | (1 << 4) | operand) & 0xfff;
            }
        }
    }

    return (cond << 28) | (I << 25) | (opcode << 21)
           | (S << 20) | (Rn << 16) | (Rd << 12) | operand;
}

int parse_multiply(char *save, Parser_Data *dat) {
    uint8_t cond, A, S, Rd, Rn, Rs, Rm;

    cond = 0xe;

    S = 0;

    char *token;

    token = strtok_r(NULL, ", ", &save);
    Rd = parse_register(token);

    token = strtok_r(NULL, ", ", &save);
    Rm = parse_register(token);

    token = strtok_r(NULL, ", \n", &save);
    Rs = parse_register(token);

    // mla
    if (strcmp(dat->mnemonic, "mla") == 0) {
        token = strtok_r(NULL, " \n", &save);
        Rn = parse_register(token);
        A = 1;
    } else {
        Rn = 0;
        A = 0;
    }

    return (cond << 28) | (A << 21) | (S << 20) | (Rd << 16)
           | (Rn << 12) | (Rs << 8) | (0x9 << 4) | Rm;
}

int parse_data_transfer(char *save, Parser_Data *dat) {
    uint8_t cond, I, P, U, L, Rn, Rd;
    int16_t offset;

    cond = 0xe;

    L = (dat->mnemonic[0] == 'l');

    char *token;

    token = strtok_r(NULL, ", ", &save);
    Rd = parse_register(token);

    token = strtok_r(NULL, "[, ]\n", &save);
    U = 1;
    if (token[0] == '=' && L == 1) {
        uint32_t expression = parse_expression(token);
        if (expression > 0xff) {
            dat->last_instr = dat->last_instr + 1;
            write_4byte_to_memory(dat->memory, expression, dat->last_instr * 4);
            P = 1;
            I = 0;
            Rn = 0xf; // PC
            offset = (dat->last_instr - dat->cuur_instr) * 4 - 8;
        } else {
            return to_move_instruction(Rd, 1, expression, 0, 0);
        }
    } else {
        I = 0;
        P = dat->pre_indexed;

        Rn = parse_register(token);

        token = strtok_r(NULL, " ,]\n", &save);
        offset = 0;
        if (token != NULL) {
            I = (token[0] != '#');
            int32_t expression = parse_expression(token);
            offset = abs(expression) & 0xffff;
            if (expression < 0) {
                U = 0;
            }
            token = strtok_r(NULL, " ,]\n", &save);
            if (token != NULL) {
                I = 1;
                uint8_t shift_type;
                uint32_t shift_operand;
                int imm = 0;
                shift_type = get_address(dat->opcode_table, token) & 0xf;

                token = strtok_r(NULL, " ,]\n", &save);
                imm = (token[0] == '#');
                shift_operand = parse_expression(token);
                if (imm) {

                    offset = ((shift_operand << 7) | (shift_type << 5) | offset) & 0xfff;
                } else {
                    offset = ((shift_operand << 8) | (shift_type << 5) | (1 << 4) | offset) & 0xfff;
                }
            }
        }
    }

    return (cond << 28) | (1 << 26) | (I << 25) | (P << 24)
           | (U << 23) | (L << 20) | (Rn << 16) | (Rd << 12)
           | offset;
}

int parse_branch(char *save, Parser_Data *dat) {
    char *token;
    uint8_t cond;
    int offset;

    cond = get_address(dat->opcode_table, dat->mnemonic) & 0xf;

    token = strtok_r(NULL, " \n", &save);
    offset = generate_branch_offset(dat->label_table, token, dat->cuur_instr * 4);

    return (cond << 28) | (0xa << 24) | (offset & 0xffffff);
}

int parse_special(char *save, Parser_Data *dat) {
    // andeq
    if (strcmp(dat->mnemonic, "andeq") == 0) {
        return 0;
    }

    // lsl

    char *token;
    uint8_t Rn;
    uint16_t operand;

    token = strtok_r(NULL, ", ", &save);
    Rn = parse_register(token);

    token = strtok_r(NULL, ", ", &save);
    operand = parse_expression(token);

    return to_move_instruction(Rn, 0, Rn, 0, operand);
}
