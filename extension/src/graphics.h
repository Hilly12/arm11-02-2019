#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "fb.h"

#define WIDTH 1024
#define HEIGHT 768
#define SIZE (WIDTH * HEIGHT)
#define DEPTH 32
#define FONT_WIDTH 8
#define FONT_HEIGHT 8
#define FONT_SIZE (FONT_WIDTH * FONT_HEIGHT)

#define BLACK   0x00000000
#define GREY    0x808080ff
#define WHITE   0xffffffff
#define RED     0xff0000ff
#define GREEN   0x00ff00ff
#define BLUE    0x0000ffff
#define CYAN    0x00ffffff
#define MAGENTA 0xff00ffff
#define YELLOW  0xffff00ff

typedef unsigned int color;

void draw_pixel(int x, int y, color c);

void draw_rectangle(int x, int y, int w, int h, color c);

void draw_image(unsigned int **image, int x, int y, int w, int h);

void draw_hline(int y, color c);

void draw_vline(int x, color c);

void draw_char(int x, int y, char letter, color c);

void draw_string(int x, int y, char* string, color c);

#endif