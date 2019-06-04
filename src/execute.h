#include "utils.h"

void executeProcessing(uint32_t *registers, DecodedInstruction *decodedInstr);

void executeMUL(uint32_t *registers, DecodedInstruction const *decodedInstr);

void executeMLA(uint32_t *registers, DecodedInstruction const *decodedInstr);

void executeTransferring(uint32_t *registers, uint8_t *memory, DecodedInstruction *decodedInstr);

void executeBranch(uint32_t *registerPC, DecodedInstruction const *decodedInstr);