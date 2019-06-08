#ifndef UTILS_H
#define UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include "definitions.h"

uint8_t checkOverflow(uint32_t const *a, uint32_t const *b, uint32_t const *result);

uint8_t isConditionSatisfied(uint32_t const *cpsr, uint8_t const *condition);

void shiftWithCarry(uint8_t const *shiftBy, uint8_t const *shiftType, uint32_t *shiftedValue, uint8_t *shiftCarry);
					
void shift(uint8_t const *shiftBy, uint8_t const *shiftType, uint32_t *shiftedValue);

void processingUpdateCPSR(uint32_t *cpsr, uint32_t const *result, uint8_t const *carry);

void multiplyingUpdateCPSR(uint32_t *cpsr, uint32_t const *result);

void gpioAccessPrint(uint32_t *address);

void load(uint32_t const *address, uint8_t const *memory, uint32_t *rdRegister);

void store(uint32_t const *address, uint8_t *memory, uint32_t const *rdRegister);

void output(uint32_t const * registers, uint8_t const * memory);

#endif
