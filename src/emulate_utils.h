#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"

// Checks if the sum of 2 integers gives an overflow (the sum is given as an argument as well)
byte check_overflow(unsigned int const *a, unsigned int const *b, unsigned int const *result);

// Checks if the condition satisfies CPSR register
byte is_condition_satisfied(unsigned int const *cpsr, byte const *condition);

// Performs a shift and finds the value of the carry bit
void shift_with_carry(byte const *shift_by, byte const *shift_type, unsigned int *shifted_value, byte *shift_carry);

// Performs a shift
void shift(byte const *shift_by, byte const *shift_type, unsigned int *shifted_value);

// Updates the value of the CPSR if S is set after a Data Processing instruction
void processing_update_CPSR(unsigned int *cpsr, unsigned int const *result, byte const *carry);

// Updates the value of the CPSR if S is set after a Multiplying instruction
void multiplying_update_CPSR(unsigned int *cpsr, unsigned int const *result);

// Prints a specific statement if we have accessed a GPIO address
void gpio_access_print(unsigned int *address);

// Loads the values allocated in a specific place in memory to a specific register
void load(unsigned int const *address, byte const *memory, unsigned int *Rd_register);

// Stores the value of a specific register to a specific place in memory
void store(unsigned int const *address, byte *memory, unsigned int const *Rd_register);

// Prints the state of our register and non-zero memory addresses
void output(unsigned int const *registers, byte const *memory);

#endif
