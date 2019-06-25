#include "graphics.h"

void draw_pixel(int x, int y, color c) {
    unsigned short (*im)[WIDTH] = (unsigned short (*)[WIDTH]) fb_get_draw_buffer();
    im[y][x] = c;
}

void draw_rectangle(int x, int y, int w, int h, color col) {
    unsigned short (*im)[WIDTH] = (unsigned short (*)[WIDTH]) fb_get_draw_buffer();
    for (int vPos = y; vPos < y + h; vPos++) {
        for (int hPos = x; hPos < x + w; hPos++) {
            im[vPos][hPos] = col;
        }
    }
}

void draw_image(color *image, int x, int y, int w, int h) {
    unsigned short (*im)[WIDTH] = (unsigned short (*)[WIDTH]) fb_get_draw_buffer();
    color col;
    for (int vPos = y, vP = 0; vPos < y + h; vPos++, vP++) {
        for (int hPos = x, hP = 0; hPos < x + w; hPos++, hP++) {
            im[vPos][hPos] = *(image + hP * h + vP);
        }
    }
}

void draw_transparent_image(color *image, int x, int y, int w, int h) {
    unsigned short (*im)[WIDTH] = (unsigned short (*)[WIDTH]) fb_get_draw_buffer();
    color col;
    for (int vPos = y, vP = 0; vPos < y + h; vPos++, vP++) {
        for (int hPos = x, hP = 0; hPos < x + w; hPos++, hP++) {
            col = *(image + hP * h + vP);
            if (col != DELIMITER) {
                im[vPos][hPos] = col;
            }
        }
    }
}

void draw_transparent_image_inverted(color *image, int x, int y, int w, int h) {
    unsigned short (*im)[WIDTH] = (unsigned short (*)[WIDTH]) fb_get_draw_buffer();
    color col;
    for (int vPos = y, vP = 0; vPos < y + h; vPos++, vP++) {
        for (int hPos = x, hP = w - 1; hPos < x + w; hPos++, hP--) {
            col = *(image + hP * h + vP);
            if (col != DELIMITER) {
                im[vPos][hPos] = col;
            }
        }
    }
}

void draw_hline(int y, color col) {
    for(int x = 0; x < WIDTH; x++)
        draw_pixel(x, y, col);
}

void draw_vline(int x, color col) {
    for(int y = 0; y < HEIGHT; y++)
        draw_pixel(x, y, col);
}

void draw_char(int x, int y, char letter, color col) {
    // Pointer to [x, y]
    unsigned short (*im)[WIDTH] = (unsigned short (*)[WIDTH]) (&((unsigned short (*)[WIDTH]) fb_get_draw_buffer())[y][x]);

    int r, c;
    for(int i = 0; i < FONT_WIDTH * FONT_HEIGHT; i++) {
        r = i / FONT_WIDTH;
        c = i % FONT_WIDTH;
        if (get_font_bit(letter, r, c)) {
            im[r][c] = col;
        }
    }
}

void draw_string(int x, int y, char *string, color col) {
    char c;
    for (int i = 0; (c = string[i]); i++) {
        draw_char(x, y, c, col);
        x += FONT_WIDTH;
    }
}

void draw_string_int(int x, int y, char *string, int val, color col) {
    char c;
    for (int i = 0; (c = string[i]); i++) {
        draw_char(x, y, c, col);
        x += FONT_WIDTH;
    }
    if (val) {
        int dig_count = 0;
        for (int i = val; i; i /= 10) {
            dig_count++;
        }
        x += FONT_WIDTH * dig_count;
        while(dig_count) {
            char c = (val % 10) + '0';
            draw_char(x, y, c, col);
            dig_count--;
            x -= FONT_WIDTH;
            val /= 10;
        }
    } else {
        x += FONT_WIDTH;
        draw_char(x, y, '0', col);
    }
}

void draw_circle(int x, int y, int d, color col) {
    unsigned short (*im)[WIDTH] = (unsigned short (*)[WIDTH]) fb_get_draw_buffer();
    int r = d / 2;
    int cx = x + r;
    int cy = y + r;

    int rx, ry;
    for (int vPos = y; vPos < y + d; vPos++) {
        for (int hPos = x; hPos < x + d; hPos++) {
            rx = hPos - cx;
            ry = vPos - cy;
            if (rx * rx + ry * ry < r * r) {
                im[vPos][hPos] = col;
            }
        }
    }
}