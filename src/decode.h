#include "utils.h"

void decodeProcessing(uint8_t const *instr, uint32_t const *registers, DecodedInstruction *decodedInstr);

void decodeMultiplying(uint8_t const *instr, DecodedInstruction *decodedInstr);

void decodeTransferring(uint8_t const *instr, uint32_t const *registers, DecodedInstruction *decodedInstr);

void decodeBranch(uint8_t const *instr, DecodedInstruction *decodedInstr);