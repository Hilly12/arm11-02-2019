#include "graphics.h"
#include "resources.h"

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
    for (int vPos = y, vP = 0; vPos < y + h; vPos++, vP++) {
        for (int hPos = x, hP = 0; hPos < x + w; hPos++, hP++) {
            im[vPos][hPos] = image[hP][vP];
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

void draw_char(int x, int y, char letter, color col) {
    // Pointer to [x, y]
    unsigned (*im)[WIDTH] = (unsigned (*)[WIDTH]) (&((unsigned (*)[WIDTH]) fb_get_draw_buffer())[y][x]);

    int r, c;
    for(int i = 0; i < FONT_WIDTH * FONT_HEIGHT; i++) {
        r = i / FONT_WIDTH;
        c = i % FONT_WIDTH;
        im[r][c] = get_font_bit(letter, r, c) ? col : 0;
    }
}

void draw_string(int x, int y, char *string, color col) {
    // char *p = string;
    // while(*p) {
    //     draw_char(x, y, *p, col);
    //     p++;
    //     x += FONT_WIDTH;
    // }
    char c;
    for (int i = 0; (c = string[i]); i++) {
        draw_char(x, y, c, col);
        x += FONT_WIDTH;
    }
}