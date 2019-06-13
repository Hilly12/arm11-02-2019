#include "fb.h"
#include "timer.h"

typedef unsigned int color;

const color WHITE = 0xffffffff;
const color RED = 0xff0000ff;
const color YELLOW = 0x00ffffff;
const unsigned WIDTH = 1024;
const unsigned HEIGHT = 768;
const unsigned DEPTH = 32;
const unsigned SQUARE_SIZE = 32;
const unsigned VISIBLE_WIDTH = 16;
const unsigned VISIBLE_HEIGHT = 12;

static rpi_sys_timer_t* rpiSystemTimer = (rpi_sys_timer_t*) RPI_SYSTIMER_BASE;

void wait_microseconds(unsigned int us) {
    volatile unsigned int start_time = rpiSystemTimer->counter_lo;
    while((rpiSystemTimer->counter_lo - start_time) < us);
}

void draw_pixel(int x, int y, color c) {
    unsigned (*im)[WIDTH] = (unsigned (*)[WIDTH]) fb_get_draw_buffer();
    im[y][x] = c;
}

void draw_rectangle(int x, int y, int w, int h, color c) {
    unsigned (*im)[WIDTH] = (unsigned (*)[WIDTH]) fb_get_draw_buffer();
    for (int vPos = y; vPos < y + h; vPos++) {
        for (int hPos = x; hPos < x + w; hPos++) {
            im[vPos][hPos] = c;
        }
    }
}

void draw_image(unsigned int **image, int x, int y, int w, int h) {
    unsigned (*im)[WIDTH] = (unsigned (*)[WIDTH]) fb_get_draw_buffer();
    for (int vPos = y; vPos < y + h; vPos++) {
        for (int hPos = x; hPos < x + w; hPos++) {
            im[vPos][hPos] = image[hPos][vPos];
        }
    }
}

void draw_hline(int y, color c) {
    for(int x = 0; x < WIDTH; x++)
        draw_pixel(x, y, c);
}

void draw_vline(int x, color c) {
    for(int y = 0; y < HEIGHT; y++)
        draw_pixel(x, y, c);
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
    
    for (int i = 0; i < 100; i++) {
        draw_rectangle(x++, 50, 100, 100, WHITE);
        wait_microseconds(1000000);
    }
}