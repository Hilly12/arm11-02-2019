#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "emulate_definitions.h"

uint8_t check_overflow(uint32_t const *a, uint32_t const *b, uint32_t const *result);

uint8_t is_condition_satisfied(uint32_t const *cpsr, uint8_t const *condition);

void shift_with_carry(uint8_t const *shift_by, uint8_t const *shift_type,
                      uint32_t *shifted_value, uint8_t *shift_carry);

void shift(uint8_t const *shift_by, uint8_t const *shift_type, uint32_t *shifted_value);

void processing_update_CPSR(uint32_t *cpsr, uint32_t const *result, uint8_t const *carry);

void multiplying_update_CPSR(uint32_t *cpsr, uint32_t const *result);

void gpio_access_print(uint32_t *address);

void load(uint32_t const *address, uint8_t const *memory, uint32_t *Rd_register);

void store(uint32_t const *address, uint8_t *memory, uint32_t const *Rd_register);

void output(uint32_t const *registers, uint8_t const *memory);

#endif
