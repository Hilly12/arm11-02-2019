#include "utils.h"

void executeDataProcessing(uint32_t *registers, uint8_t const *instr);

void executeMultiply(uint32_t *registers, uint8_t const *instr);

void executeSingleDataTransfer(uint32_t *registers, uint8_t *memory, uint8_t const *instr);

void executeBranch(uint32_t *registers, uint8_t const *instr);