#include <stdint.h>
#include "fb.h"
#include "timer.h"
#include "graphics.h"

#define REFRESH_RATE 80000 // 1000000 / 80000 = 12.5 FPS (Recommended for 16 bit color)

uint32_t frame_count;

// Initialize state of program here
void start() {
    // Code here
}

// Get user input here and make necessary modifications to state
void get_input() {
    // Code here
}

// Update state of program per frame
void update() {
    get_input();
    // Code here
}

// Clear the screen
void clear() {
    draw_rectangle(0, 0, WIDTH, HEIGHT, BLACK);
}

// Render 
void draw() {
    // Code here
    fb_swap_buffer();
}

// A cool fade intro to your project
// Modify coords depending on resolution
void fade_intro() {
    char name[] = " A Roguelike\0";
    char intro[] = " Brought to you by the Kernel Gang\0";

    unsigned short col = 0xFFDF; // 1111 1111 1101 1111
    while (col) {
        draw_string(276, 228, name, col);
        draw_string(188, 240, intro, col);
        fb_swap_buffer();
        timer_sleep(120000);
        // 0000 1000 0100 0001 (1 from r, b; 2 from g)
        col -= 0x0841;
    }
    clear();
    fb_swap_buffer();
    clear();
    timer_sleep(1000000);
}

void main(void) {
    fb_init(WIDTH, HEIGHT, DEPTH, FB_DOUBLEBUFFER);

    fade_intro();

    start();

    // Game Loop
    while (1) {
        update();
        draw();
        frame_count++;
        timer_sleep(REFRESH_RATE);
    }
}