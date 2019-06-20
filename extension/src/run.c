#include "fb.h"
#include "timer.h"
#include "graphics.h"
#include "resources.h"
#include "map_generator.h"
#include "enemy.h"

#define MAX_STEPS 2
#define BASE_HP 100
#define BASE_STATS 3

uint32_t frame_count;
uint8_t flr = 0;
uint8_t player_x;
uint8_t player_y;
uint8_t direction; // LEFT, RIGHT, UP, DOWN
uint8_t moving_left, moving_right, moving_up, moving_down;
uint8_t moving;
uint8_t facing_left;
uint8_t state; // IDLE / ANIMATING
uint8_t animation_steps;
uint8_t lvl;
uint8_t maxhp;
uint8_t hp;
uint8_t atk;
uint8_t def;
uint8_t lck;
uint8_t enemy_count;
unsigned long long enemies[MAX_ENEMIES] = { 0 };

uint8_t map[MAP_WIDTH][MAP_HEIGHT] = { 0 }; // 1 entry = Blocked | Excess Wall | Tile Sprite | Tile Sprite | Tile type

char *floor_pre = " Floor:\0";

void new_floor() {
    flr++;
    frame_count = 0;
    direction = 1;
    state = 0;
    animation_steps = 0;
    moving = moving_left = moving_right = moving_up = moving_down = 0;
    moving_up = 1;
    facing_left = 0;
    generate_map(map[0], 5, &player_x, &player_y);
    enemy_count = 5 + get_random(0, flr / 7);
    spawn_enemies(enemies, map[0], player_x, player_y, flr, enemy_count);
}

void start() {
    lvl = 1;
    maxhp = hp = BASE_HP;
    atk = def = lck = BASE_STATS;
    new_floor();
}

void update() {
    if (state) { // ANIMATING
        animation_steps++;
        if (animation_steps >= MAX_STEPS) {
            animation_steps = 0;
            if (direction == 0) {
                player_x--;
            } else if (direction == 1) {
                player_x++;
            } else if (direction == 2) {
                player_y--;
            } else if (direction == 3) {
                player_y++;
            }
            moving = 0;
            state = 0;
        }
    } else { // IDLE
        if (moving_left && is_movable(map[0], player_x - 1, player_y)) {
            direction = 0;
            map[player_x - 1][player_y] |= 1 << 4; // block
            moving = 1;
        } else if (moving_right && is_movable(map[0], player_x + 1, player_y)) {
            direction = 1;
            map[player_x + 1][player_y] |= 1 << 4;
            moving = 1;
        } else if (moving_up && is_movable(map[0], player_x, player_y - 1)) {
            direction = 2;
            map[player_x][player_y - 1] |= 1 << 4;
            moving = 1;
        } else if (moving_down && is_movable(map[0], player_x, player_y + 1)) {
            direction = 3;
            map[player_x][player_y + 1] |= 1 << 4;
            moving = 1;
        }
        if (moving) {
            map[player_x][player_y] &= 0xEF; // 1110 1111 - unblock
            state = 1;
        }
    }
}

void clear() {
    draw_rectangle(0, 0, WIDTH, HEIGHT, BLACK);
    // fb_clear(SIZE);
}

void draw() {
    int x_shift = 0;
    int y_shift = 0;
    int movement_shift = (SQUARE_SIZE * animation_steps / MAX_STEPS);
    if (moving) {
        if (direction & 1) { // Right, Down
            x_shift = movement_shift * (direction == 1);
            y_shift = movement_shift * (direction == 3);
        } else { // Left, Up
            x_shift = -movement_shift * (direction == 0);
            y_shift = -movement_shift * (direction == 2);
        }
    }
    for (int x = 0; x < VISIBLE_WIDTH; x++) {
        if (x == 0 && x_shift > 0) {
            continue;
        }
        if (x == VISIBLE_WIDTH - 1 && x_shift < 0) {
            continue;
        }
        for (int y = 0; y < VISIBLE_HEIGHT; y++) {
            if (y == 0 && y_shift > 0) {
                continue;
            }
            if (y == VISIBLE_HEIGHT - 1 && y_shift < 0) {
                continue;
            }

            int rx = x + player_x - HCENT;
            int ry = y + player_y - VCENT;
            if (is_valid(rx, ry) && !((map[rx][ry] >> 3) & 1)) {
                if (map[rx][ry] & 1) {
                    draw_image(get_wall_tile((map[rx][ry] >> 1) & 3),
                     x * SQUARE_SIZE - x_shift,
                     y * SQUARE_SIZE - y_shift,
                     SQUARE_SIZE, SQUARE_SIZE);
                } else {
                    draw_image(get_floor_tile((map[rx][ry] >> 1) & 3),
                     x * SQUARE_SIZE - x_shift,
                     y * SQUARE_SIZE - y_shift,
                     SQUARE_SIZE, SQUARE_SIZE);
                }
            } else {
                draw_rectangle(x * SQUARE_SIZE - x_shift, y * SQUARE_SIZE - y_shift, SQUARE_SIZE, SQUARE_SIZE, BLACK);
            }
        }
    }

    if (facing_left) {
        draw_transparent_image(get_player((frame_count >> 1) & 1), HCENT * SQUARE_SIZE, VCENT * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
    } else {
        draw_transparent_image_inverted(get_player((frame_count >> 1) & 1), HCENT * SQUARE_SIZE, VCENT * SQUARE_SIZE, SQUARE_SIZE, SQUARE_SIZE);
    }

    draw_rectangle(0, 0, WIDTH, SQUARE_SIZE, 0x3A8B);
    draw_rectangle(0, HEIGHT - SQUARE_SIZE, WIDTH, SQUARE_SIZE, 0x3A8B);
    draw_rectangle(0, 0, SQUARE_SIZE, HEIGHT, 0x3A8B);
    draw_rectangle(WIDTH - SQUARE_SIZE, 0, SQUARE_SIZE, HEIGHT, 0x3A8B);
    draw_string_int(10, 10, floor_pre, flr, 0xBD53);

    fb_swap_buffer();
}

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

void graphics_demo() {
    char ob[] = " Oh Baby!\0";
    int obx = 200;
    int obvel = 2;

    char wt[] = " Where is true?\0";
    unsigned short wtcol = 0xFFFF;

    int py = 300, pvel = 0, pg = 1, frn = 800, frd = 1000;

    int cx = 10, cy = 50, cxvel = -4, cyvel = 4;

    while (1) {
        clear();

        draw_transparent_image(get_player(0), 400, py, SQUARE_SIZE, SQUARE_SIZE);
        py += pvel;
        pvel += pg;
        if (py + SQUARE_SIZE >= HEIGHT) {
            py -= pvel;
            pvel = -pvel * frn / frd;
        }

        draw_string(100, 10, wt, wtcol);
        wtcol -= 0x1111;

        draw_circle(cx, cy, 20, 0x51D1);
        cx += cxvel;
        cy += cyvel;
        if (cx <= 0) {
            cx = 0;
            cxvel = 4 + (get_random(0, 3) - 1); // 4 +/- 1
        }
        if (cy <= 0) {
            cy = 0;
            cyvel = 4 + (get_random(0, 3) - 1); // 4 +/- 1
        }
        if (cx + 20 >= WIDTH) {
            cx -= cxvel;
            cxvel = -4 + (get_random(0, 3) - 1); // -4 +/- 1
        }
        if (cy + 20 >= HEIGHT) {
            cy -= cyvel;
            cyvel = -4 + (get_random(0, 3) - 1); // 4 +/- 1
        }

        draw_string(obx, 150, ob, GREEN);
        obx += obvel;
        if (obx <= 0) {
            obx = 0;
            obvel = 4;
        }
        if (obx + 8 * 10 >= WIDTH) {
            obx = WIDTH - 8 * 10 - 1;
            obvel = -4;
        }

        fb_swap_buffer();
        timer_sleep(50000);
    }
}

void main(void) {
    fb_init(WIDTH, HEIGHT, DEPTH, FB_DOUBLEBUFFER);

    fade_intro();

    graphics_demo();

    start();

    while (1) {
        update();
        draw();
        frame_count++;
        timer_sleep(120000);
    }

    while(1);
}