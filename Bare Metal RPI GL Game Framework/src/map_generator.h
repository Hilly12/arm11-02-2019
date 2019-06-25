#ifndef MAP_GENERATOR_H
#define MAP_GENERATOR_H

#define MAP_WIDTH 128
#define MAP_HEIGHT 72
#define VISIBLE_WIDTH 20
#define VISIBLE_HEIGHT 15
#define SQUARE_SIZE (WIDTH / VISIBLE_WIDTH)
#define HCENT 9
#define VCENT 7
#define MAX_ROOMS_EVER 30

#include <stdint.h>
#include "random.h"

typedef struct room {
    uint8_t x1;
    uint8_t x2;
    uint8_t y1;
    uint8_t y2;
} room_t;

void generate_map(uint8_t *map, uint8_t floor, uint8_t *player_x, uint8_t *player_y);

int intersect(room_t *r1, room_t *r2);

void h_corridor(uint8_t *map, uint8_t x1, uint8_t x2, uint8_t y);

void v_corridor(uint8_t *map, uint8_t y1, uint8_t y2, uint8_t x);

int is_valid(int x, int y);

int is_movable(uint8_t *map, int x, int y);

int are_adjacent(int x1, int y1, int x2, int y2);

int calc_dist_sq(int x1, int y1, int x2, int y2);

int get_surrounding_floor_tiles(uint8_t *map, uint8_t hPos, uint8_t vPos);

#endif