#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"

uint8_t getBit(uint32_t src, uint8_t amt);

uint32_t rotateRight32(uint32_t src, uint32_t amt);

uint8_t isConditionSatisfied(uint32_t cpsr, uint8_t condition);

void barrelShift(uint32_t const *registers, uint16_t input,
                 uint32_t *output, uint8_t *shiftCarry);

void output(uint32_t const * registers, uint8_t const * memory);

#endif