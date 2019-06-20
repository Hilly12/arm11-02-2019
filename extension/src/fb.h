#ifndef FB_H
#define FB_H

#define FB_SINGLEBUFFER 0
#define FB_DOUBLEBUFFER 1

#define GPU_NOCACHE 0x40000000

void fb_init(unsigned width, unsigned height, unsigned depth,
             unsigned doublebuffer);
void fb_clear(unsigned int size);


unsigned fb_get_depth(void);
unsigned fb_get_height(void);
unsigned fb_get_width(void);

unsigned char* fb_get_draw_buffer(void);
void fb_swap_buffer(void);

#endif
