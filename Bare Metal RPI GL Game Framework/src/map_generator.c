#include "map_generator.h"

uint32_t rooms[MAX_ROOMS_EVER] = { 0 };

void generate_map(uint8_t *map, uint8_t floor, uint8_t *player_x, uint8_t *player_y) {
    // Initialize map
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            *(map + x * MAP_HEIGHT + y) = 1;
        }
    }

    *player_x = 50;
    *player_y = 50;

    int min_rooms = 3 + (floor / 10);
    int max_rooms = 5 + (floor / 5);
    int room_count = get_random(min_rooms, max_rooms + 1);
    int x, y, w, h;
    room_t *r;
    for (int i = 0; i < room_count; i++) {
        w = get_random(5, 18);
        h = get_random(5, 14);

        x = get_random(0, MAP_WIDTH - w - 1) + 1;
        y = get_random(0, MAP_HEIGHT - h - 1) + 1;

        r = (room_t *) &rooms[i];
        r->x1 = x;
        r->x2 = x + w;
        r->y1 = y;
        r->y2 = y + h;

        int failed = 0;
        for (int j = 0; j < i; j++) {
            if (intersect((room_t *) &rooms[j], r)) {
                failed = 1;
                i--;
                break;
            }
        }
        if (!failed) {
            if (i != 0) {
                room_t *prev = (room_t *) &rooms[i - 1];
                uint8_t pcx = (prev->x1 + prev->x2) / 2;
                uint8_t pcy = (prev->y1 + prev->y2) / 2;
                uint8_t cx = (r->x1 + r->x2) / 2;
                uint8_t cy = (r->y1 + r->y2) / 2;
                if (get_random(0, 2)) {
                    h_corridor(map, pcx, cx, pcy);
                    v_corridor(map, pcy, cy, cx);
                } else {
                    v_corridor(map, pcy, cy, pcx);
                    h_corridor(map, pcx, cx, cy);
                }
            }
        }
    }
    
    // Place player and stairs
    int n = get_random(0, room_count);
    r = (room_t *) &rooms[n];
    *player_x = (r->x1 + r->x2) / 2;
    *player_y = (r->y1 + r->y2) / 2;

    // Clear map
    for (int i = 0; i < room_count; i++) {
        r = (room_t *) &rooms[i];
        for (int x = r->x1; x < r->x2; x++) {
            for (int y = r->y1; y < r->y2; y++) {
                *(map + x * MAP_HEIGHT + y) = 0;
            }
        }
    }

    // Remove excess walls and set sprite type
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            *(map + x * MAP_HEIGHT + y) &= 0x0F;
            if ((*(map + x * MAP_HEIGHT + y) & 1) && !get_surrounding_floor_tiles(map, x, y)) {
                *(map + x * MAP_HEIGHT + y) |= 1 << 3;
            } else {
                *(map + x * MAP_HEIGHT + y) |= (get_random(0, 4) << 1);
            }
        }
    }
}

int intersect(room_t *r1, room_t *r2) {
    return (r1->x1 <= r2->x2) && (r2->x1 <= r1->x2) && (r1->y1 <= r2->y2) && (r2->y1 <= r1->y2);
}

void h_corridor(uint8_t *map, uint8_t x1, uint8_t x2, uint8_t y) {
    int a, b;
    if (x1 <= x2) {
        a = x1;
        b = x2;
    } else {
        b = x1;
        a = x2;
    }
    for (int x = a; x <= b; x++) {
        *(map + x * MAP_HEIGHT + y) = 0;
    }
}

void v_corridor(uint8_t *map, uint8_t y1, uint8_t y2, uint8_t x) {
    uint8_t a, b;
    if (y1 <= y2) {
        a = y1;
        b = y2;
    } else {
        b = y1;
        a = y2;
    }
    for (int y = a; y <= b; y++) {
        *(map + x * MAP_HEIGHT + y) = 0;
    }
}

int is_valid(int x, int y) {
    return 0 <= x && x < MAP_WIDTH && 0 <= y && y < MAP_HEIGHT;
}

int is_movable(uint8_t *map, int x, int y) {
    uint8_t e = *(map + x * MAP_HEIGHT + y);
    return !(e & 1) && !((e >> 4) & 1);
}

int are_adjacent(int x1, int y1, int x2, int y2) {
    return (((x1 - x2) * (x1 - x2) == 1) && ((y1 - y2) == 0))
        || (((y1 - y2) * (y1 - y2) == 1) && ((x1 - x2) == 0));
}

int calc_dist_sq(int x1, int y1, int x2, int y2) {
    return (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
}

int get_surrounding_floor_tiles(uint8_t *map, uint8_t hPos, uint8_t vPos) {
    int count = 0;
    for (int y = vPos - 1; y <= vPos + 1; y++) {
        for (int x = hPos - 1; x <= hPos + 1; x++) {
            if (is_valid(x, y) && !(*(map + x * MAP_HEIGHT + y) & 1)) {
                count++;
            }
        }
    }
    return count;
}