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

// N.B.
// All graphics functions are meant to render fast and 
// hence do not take screen bounds into consideration
// Hence trying to draw something beyond the length of
// a line could result in it rendering on the next line
// This can be changed with minor modifications - adding 
// conditionals - which shouldn't hinder performance too
// much with 16 bit color

// All images are currently 16 bit, in order to use them
// in 32 bit form or use other images in general,
// you would have to copy the RGB values of each
// individual pixel into a 2D array

// Alternatively if you want to read bitmaps,
// you can read their color values into a
// 2D color array and pass the address of the
// first element in the array

// Draws pixel of color c 
void draw_pixel(int x, int y, color c);

// Draws rectangle of color c and dimensions w x h at [x, y]
void draw_rectangle(int x, int y, int w, int h, color c);

// Draws image stored in color array spanning dimensions w x h at [x, y]
// PRE - image must point to the first element in an 2D array of dimensions [w][h]
void draw_image(color *image, int x, int y, int w, int h);

// Draws all pixels apart from those with color equivalent to DELIMITER
// PRE - image must point to the first element in an 2D array of dimensions [w][h]
void draw_transparent_image(color *image, int x, int y, int w, int h);

// Same as draw_transparent_image but horizontally inverted
// PRE - image must point to the first element in an 2D array of dimensions [w][h]
void draw_transparent_image_inverted(color *image, int x, int y, int w, int h);

// Draws horizontal line of color c across the the screen at y coord
void draw_hline(int y, color c);

// Draws vertical line of color c down the the screen at x coord
void draw_vline(int x, color c);

// Draws character letter on the screen according to predefined 8 x 8 font at [x, y]
// This font is defined in "resources.c"
void draw_char(int x, int y, char letter, color col);

// Draws a string on the screen at [x, y] of height 8 and width strlen(string) * 8
// Note to draw string "Hello" you must declare the string in the form:
// char *str = { 'H', 'e', 'l', 'l', 'o', '\0' }; (or)
// char *str = "-Hello\0";
// Note the first character is ignored here, and the sentinel is usually unnecessary
void draw_string(int x, int y, char *string, color col);

// Draws a string as in draw_string but followed by the integer val
// PRE -- val >= 0
void draw_string_int(int x, int y, char *string, int val, color col);

// Draws a circle of diameter d and color c at [x, y]
void draw_circle(int x, int y, int d, color col);

#endif