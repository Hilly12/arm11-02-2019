#include "fb.h"
#include "timer.h"
#include "graphics.h"
#include "resources.h"

#define RPI_SYSTIMER_BASE 0x20003000

typedef struct {
    volatile unsigned int control_status;
    volatile unsigned int counter_lo;
    volatile unsigned int counter_hi;
    volatile unsigned int compare0;
    volatile unsigned int compare1;
    volatile unsigned int compare2;
    volatile unsigned int compare3;
} rpi_sys_timer_t;

static rpi_sys_timer_t* rpiSystemTimer = (rpi_sys_timer_t*) RPI_SYSTIMER_BASE;

void wait_microseconds(unsigned int us) {
    volatile unsigned int start_time = rpiSystemTimer->counter_lo;
    while((rpiSystemTimer->counter_lo - start_time) < us);
}

void main(void) {
    fb_init(WIDTH, HEIGHT, DEPTH, FB_SINGLEBUFFER);
    
    // for(int y = 0; y < HEIGHT; y += 16) { 
    //     draw_hline(y, RED);
    // }
    // for(int x = 0; x < WIDTH; x += 16) {
    //     draw_vline(x, YELLOW);
    // }

    int x = 100;

    char str[] = "hello world";
    
    draw_rectangle(0, 0, SQUARE_SIZE, SQUARE_SIZE, MAGENTA);

    draw_string(100, 100, str, CYAN);

    draw_image(get_player0(), 300, 100, 32, 32);

    // while (x + 100 < WIDTH) {
    //     fb_clear(0, SIZE);
    //     draw_rectangle(x, 50, 100, 100, WHITE);
    //     timer_sleep(2000000);
    //     // wait_microseconds(1000000);
    //     x++;
    // }

    while(1);
}