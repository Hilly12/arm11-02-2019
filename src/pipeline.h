#include "execute.h"

void fetch(uint32_t pcVal, uint8_t const *memory, uint8_t *instr);

void decode(uint8_t const *instr, decodedInstruction *decodedInstr);

void execute(uint32_t *registers, uint8_t *memory,
             decodedInstruction decodedInstr,
             uint8_t *branched, uint8_t *finished);