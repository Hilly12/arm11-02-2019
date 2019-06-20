#ifndef RANDOM_H
#define RANDOM_H

#define MMIO_BASE       0x3F000000
#define RNG_CTRL        ((volatile unsigned int*)(MMIO_BASE+0x00104000))
#define RNG_STATUS      ((volatile unsigned int*)(MMIO_BASE+0x00104004))
#define RNG_DATA        ((volatile unsigned int*)(MMIO_BASE+0x00104008))
#define RNG_INT_MASK    ((volatile unsigned int*)(MMIO_BASE+0x00104010))

void rand_init(void);

unsigned int get_random(unsigned int min, unsigned int max);

#endif