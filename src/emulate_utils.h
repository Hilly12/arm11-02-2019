#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"

byte check_overflow(unsigned int const *a, unsigned int const *b, unsigned int const *result);

byte is_condition_satisfied(unsigned int const *cpsr, byte const *condition);

void shift_with_carry(byte const *shift_by, byte const *shift_type,
                      unsigned int *shifted_value, byte *shift_carry);

void shift(byte const *shift_by, byte const *shift_type, unsigned int *shifted_value);

void processing_update_CPSR(unsigned int *cpsr, unsigned int const *result, byte const *carry);

void multiplying_update_CPSR(unsigned int *cpsr, unsigned int const *result);

void gpio_access_print(unsigned int *address);

void load(unsigned int const *address, byte const *memory, unsigned int *Rd_register);

void store(unsigned int const *address, byte *memory, unsigned int const *Rd_register);

void output(unsigned int const *registers, byte const *memory);

#endif
