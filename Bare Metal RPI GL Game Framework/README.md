# Bare Metal RPI GL Game Framework

This was created with the intention of allowing future students attempting this project to skip the stage of creating a graphics library and supporting detailed animations so they can jump into creating games immediately.

The Graphics library follows the following functions: (more detail in graphics.h)
```
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
```

There is also a Map Generator module which supports procedural dungeon generation.

The game logic has been abstracted into a loop calling functions which you can use to modify the state of the program per frame.
```
start(); // Initialize state
// Game Loop
while (1) {
  update(); // Update state per frame
  draw(); // Render onto screen
  frame_count++;
  timer_sleep(REFRESH_RATE);
}
```

Note: 
You should only need to modify the code in run.c
Keep in mind that this was created for the Raspberry Pi B 2011 model however, in order to get it to run on a newer model you would have to modify some of the '#define's in modules such as timer.c since the RPI timer may be in a different address in memory.

## Compilation

COMPILE SINGLE .c FILE INTO .elf FILE
```
arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles file.c -o kernel.elf
```

COMPILE MULTIPLE .s and .c FILES INTO .elf FILE
```
arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s -nostartfiles *.s *.c -o kernel.elf
```
-- USE THIS IF THE ABOVE DOESN'T WORK
```
arm-none-eabi-gcc -O2 -mfpu=vfp -mfloat-abi=soft -march=armv6zk -mtune=arm1176jzf-s -nostartfiles *.s *.c -o kernel.elf
```

CONVERT .elf into .img
```
arm-none-eabi-objcopy kernel.elf -O binary kernel.img
```
