#ifndef DEFINITIONS_H
#define DEFINITIONS_H

// Memory size and number of registers
#define MEMORY_SIZE 65536
#define REGISTERS 17

// References to important registers
#define PC_REF 15
#define CPSR_REF 16

// Conditions
#define EQ 0
#define NE 1
#define GE 10
#define LT 11
#define GT 12
#define LE 13
#define AL 14

// Types of instruction
#define ANDEQ 0
#define INVALID 1
#define PROCESSING 2
#define MULTIPLYING 3
#define TRANSFER 4
#define BRANCH 5

// Number representing the entry of the array instr[] that contains the value formed by the bits of the specified range
#define BITS_28_TO_31 0
#define BITS_24_TO_27 1
#define BITS_20_TO_23 2
#define BITS_16_TO_19 3
#define BITS_12_TO_15 4
#define BITS_8_TO_11 5
#define BITS_4_TO_7 6
#define BITS_0_TO_3 7

// Opcode
#define AND 0
#define EOR 1
#define SUB 2
#define RSB 3
#define ADD 4
#define TST 8
#define TEQ 9
#define CMP 10
#define ORR 12
#define MOV 13

// Shift types
#define LSL 0
#define LSR 1
#define ASR 2
#define ROR 3

// Branch types
#define BEQ 0
#define BNE 1
#define BGE 10
#define BLT 11
#define BGT 12
#define BLE 13
#define B 14

// Numbers of GPIO bytes for different purposes and GPIO addresses
#define GPIOBYTES 12
#define GPIO_ON_OFF_BYTES 4
#define GPIO_0_9_FIRST_BYTE 0x20200000
#define GPIO_0_9_LAST_BYTE 0x20200003
#define GPIO_10_19_LAST_BYTE 0x20200007
#define GPIO_20_29_FIRST_BYTE 0x20200008
#define GPIO_20_29_LAST_BYTE 0x2020000B
#define GPIO_ON_FIRST_BYTE 0x2020001C
#define GPIO_OFF_FIRST_BYTE 0x20200028

// Bitmasks for getting specific bits
#define GET_BIT_1 0x1
#define GET_BIT_3 0x4

// Bitmasks for getting the first 'n' bits
#define GET_FIRST_2_BITS 0x3
#define GET_FIRST_4_BITS 0xF
#define GET_FIRST_8_BITS 0xFF
#define GET_FIRST_12_BITS 0xFFF
#define GET_FIRST_16_BITS 0xFFFF
#define GET_FIRST_24_BITS 0xFFFFFF

// Bitmasks for clearing bits of the CPSR
#define BITMASK_CLEAR_N 0x7FFFFFFF
#define BITMASK_CLEAR_Z 0xBFFFFFFF
#define BITMASK_CLEAR_NZ 0x3FFFFFFF
#define BITMASK_CLEAR_NC 0x5FFFFFFF
#define BITMASK_CLEAR_ZC 0x9FFFFFFF
#define BITMASK_CLEAR_NZC 0x1FFFFFFF

// Bitmasks for setting bits of the CPSR
#define BITMASK_SET_N 0x80000000
#define BITMASK_SET_Z 0x40000000
#define BITMASK_SET_C 0x20000000
#define BITMASK_SET_NC 0xA0000000
#define BITMASK_SET_ZC 0x60000000

// Parse types
#define PARSETYPE_0 0
#define PARSETYPE_1 1
#define PARSETYPE_2 2
#define PARSETYPE_3 3
#define PARSETYPE_4 4

typedef unsigned char byte;

#endif
