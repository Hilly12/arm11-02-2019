#include "decode.h"
#include "execute.h"

void fetch(uint32_t const *pcVal, uint8_t const *memory, uint8_t *instr);

void decode(uint8_t const *instr, uint32_t const *registers, DecodedInstruction *decodedInstr);

void execute(uint32_t *registers, uint8_t *memory, DecodedInstruction *decodedInstr);