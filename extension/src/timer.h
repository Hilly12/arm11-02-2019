// #include <stdint.h>
// #define RPI_SYSTIMER_BASE 0x20003000

// typedef struct {
//     volatile unsigned int control_status;
//     volatile unsigned int counter_lo;
//     volatile unsigned int counter_hi;
//     volatile unsigned int compare0;
//     volatile unsigned int compare1;
//     volatile unsigned int compare2;
//     volatile unsigned int compare3;
// } rpi_sys_timer_t;

#ifndef __TIMER_H__
#define __TIMER_H__

#define RPI_TIMER_BASE 0x20003000
#define RPI_TIMER_COUNTER 0x04

extern int timer_fps_current;

#define TIMER_SEC 1000000

void timer_count_frame();
void timer_sleep(unsigned long long delay);

#endif