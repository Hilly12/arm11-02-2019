#include "random.h"

void rand_init(void) {
    *RNG_STATUS=0x40000;
    *RNG_INT_MASK|=1; // Mask interrupt
    *RNG_CTRL|=1; // Enable
    while(!((*RNG_STATUS)>>24)) asm volatile("nop"); // Wait to gain entropy
}

unsigned int rand(unsigned int min, unsigned int max) {
    return *RNG_DATA % (max-min) + min;
}