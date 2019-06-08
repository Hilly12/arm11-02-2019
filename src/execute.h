#include "utils.h"

void executeProcessing(uint32_t *registers, DecodedInstruction *decodedInstr);

void executeMultiplying(uint32_t *registers, DecodedInstruction const *decodedInstr);

void executeTransferring(uint32_t *registers, uint8_t *memory, uint8_t *gpio, uint8_t *gpio_on,
        uint8_t *gpio_off, DecodedInstruction *decodedInstr);

void executeBranch(uint32_t *registerPC, DecodedInstruction const *decodedInstr);
