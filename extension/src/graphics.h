#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include "fb.h"
#include "resources.h"
#include <stdlib.h>

#define WIDTH 640
#define HEIGHT 480
#define SIZE (WIDTH * HEIGHT)
#define DEPTH 16
#define FONT_WIDTH 8
#define FONT_HEIGHT 8
#define FONT_SIZE (FONT_WIDTH * FONT_HEIGHT)

// 32 bit colours
// #define BLACK   0x000000ff
// #define D_GREY  0x404040ff
// #define GREY    0x808080ff
// #define L_GREY  0xc0c0c0ff
// #define WHITE   0xffffffff
// #define RED     0xff0000ff
// #define GREEN   0x00ff00ff
// #define BLUE    0x0000ffff
// #define CYAN    0x00ffffff
// #define MAGENTA 0xff00ffff
// #define YELLOW  0xffff00ff
// typedef unsigned int color;

#define BLACK   0x0000
#define D_GREY  0x4208
#define GREY    0x8410
#define L_GREY  0xC618
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define DELIMITER 0x07A0

typedef unsigned short color;

void draw_pixel(int x, int y, color c);

void draw_rectangle(int x, int y, int w, int h, color c);

void draw_image(color *image, int x, int y, int w, int h);

// Draws all pixels apart from those with color equivalent to DELIMITER
void draw_transparent_image(color *image, int x, int y, int w, int h);

void draw_transparent_image_inverted(color *image, int x, int y, int w, int h);

void draw_hline(int y, color c);

void draw_vline(int x, color c);

void draw_char(int x, int y, char letter, color col);

void draw_string(int x, int y, char *string, color col);

// Draws string followed by integer
void draw_string_int(int x, int y, char *string, int val, color col);

void draw_circle(int x, int y, int d, color col);

#endif