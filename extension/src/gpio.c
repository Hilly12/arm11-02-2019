#define GPIO_BASE 0x20200000UL

#define GPIO_GPFSEL0 0
#define GPIO_GPFSEL1 1
#define GPIO_GPFSEL2 2
#define GPIO_GPFSEL3 3
#define GPIO_GPFSEL4 4
#define GPIO_GPFSEL5 5

#define GPIO_GPSET0 7
#define GPIO_GPSET1 8

#define GPIO_GPCLR0 10
#define GPIO_GPCLR1 11

#define GPIO_GPLEV0 13
#define GPIO_GPLEV1 14

#define GPIO_GPEDS0 16
#define GPIO_GPEDS1 17

#define GPIO_GPREN0 19
#define GPIO_GPREN1 20

#define GPIO_GPFEN0 22
#define GPIO_GPFEN1 23

#define GPIO_GPHEN0 25
#define GPIO_GPHEN1 26

#define GPIO_GPLEN0 28
#define GPIO_GPLEN1 29

#define GPIO_GPAREN0 31
#define GPIO_GPAREN1 32

#define GPIO_GPAFEN0 34
#define GPIO_GPAFEN1 35

#define GPIO_GPPUD 37
#define GPIO_GPPUDCLK0 38
#define GPIO_GPPUDCLK1 39

#define LED_GPFSEL GPIO_GPFSEL1
#define LED_GPFBIT 18
#define LED_GPSET GPIO_GPSET0
#define LED_GPCLR GPIO_GPCLR0
#define LED_GPIO_BIT 16

#define RPI_SYSTIMER_BASE 0x20003000

#include <stdint.h>

typedef struct {
    volatile uint32_t control_status;
    volatile uint32_t counter_lo;
    volatile uint32_t counter_hi;
    volatile uint32_t compare0;
    volatile uint32_t compare1;
    volatile uint32_t compare2;
    volatile uint32_t compare3;
} rpi_sys_timer_t;

volatile unsigned int *gpio = (unsigned int *) GPIO_BASE;
static rpi_sys_timer_t *rpiSystemTimer = (rpi_sys_timer_t *) RPI_SYSTIMER_BASE;

void RPI_WaitMicroSeconds(uint32_t us) {
    volatile uint32_t start_time = rpiSystemTimer->counter_lo;
    while ((rpiSystemTimer->counter_lo - start_time) < us);
}

void light_pi(int n) {
    volatile unsigned int i;
    gpio[LED_GPFSEL] |= (1 << LED_GPFBIT);

    for (i = 0; i < n; i++) {
        RPI_WaitMicroSeconds(1000000); // wait 1 second

        gpio[LED_GPCLR] = (1 << LED_GPIO_BIT);

        RPI_WaitMicroSeconds(1000000);

        gpio[LED_GPSET] = (1 << LED_GPIO_BIT);
    }
}