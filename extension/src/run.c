#include "fb.h"
#include "timer.h"
#include "graphics.h"
#include "resources.h"
#include "map_generator.h"

int player_x;
int player_y;
int z = -1;
char **map[MAP_WIDTH][MAP_HEIGHT];

void start() {
    player_x = 0;
    player_y = 0;
    generate_map((char **) map, 5, &player_x, &player_y);
}

void update() {
    // map[player_x - 1][player_y - 1] = (map[player_x - 1][player_y - 1]) + z;
    // z = -z;
}

void draw() {
    fb_clear(0, SIZE);
    for (int x = 0; x < VISIBLE_WIDTH; x++) {
        for (int y = 0; y < VISIBLE_HEIGHT; y++) {
            int rx = x + player_x - HCENT;
            int ry = y + player_y - VCENT;
            if (is_valid(x, y)) {
                draw_rectangle(x * SQUARE_SIZE, y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, WHITE);
                draw_char(x * SQUARE_SIZE, y * SQUARE_SIZE, 'a', RED);
            } else {
                draw_rectangle(x * SQUARE_SIZE, y * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, GREY);
            }
        }
    }

    draw_rectangle(HCENT * SQUARE_SIZE, VCENT * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE, RED);
}

void main(void) {
    fb_init(WIDTH, HEIGHT, DEPTH, FB_SINGLEBUFFER);

    start();

    while (1) {
        update();
        draw();
        timer_sleep(1000000);
    }

    while(1);
}