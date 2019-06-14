#include "map_generator.h"
#include "random.h"

void generate_map(char **map, int floor, int *player_x, int *player_y) {

    // Initialize map
    for (int x = 0; x < MAP_WIDTH; x++) {
        for (int y = 0; y < MAP_HEIGHT; y++) {
            map[x][y] = 1;
        }
    }

    // // Place Rooms
    // int min_rooms = 3 + (floor / 10);
    // int max_rooms = 4 + (floor / 5);
    // int room_count = rand(min_rooms, max_rooms);
    // int x, y, w, h;
    // room rooms[room_count];
    // for (int i = 0; i < room_count; i++) {
    //     w = rand(5, 18);
    //     h = rand(5, 14);

    //     x = rand(0, MAP_WIDTH - w - 1) + 1;
    //     y = rand(0, MAP_HEIGHT - h - 1) + 1;

    //     rooms[i].x1 = x;
    //     rooms[i].x2 = x + w;
    //     rooms[i].y1 = y;
    //     rooms[i].y2 = y + h;
    //     rooms[i].cx = (x + x + w) / 2;
    //     rooms[i].cy = (y + y + h) / 2;

    //     int failed = 0;
    //     for (int j = 0; j < i; j++) {
    //         if (intersect(rooms[j], rooms[i])) {
    //             failed = 1;
    //             i--;
    //             break;
    //         }
    //     }
    //     if (!failed) {
    //         if (i != 0) {
    //             if (rand(0, 2)) {
    //                 h_corridor((char **) map, rooms[i - 1].cx, rooms[i].cx, rooms[i - 1].cy);
    //                 v_corridor((char **) map, rooms[i - 1].cy, rooms[i].cy, rooms[i].cx);
    //             } else {
    //                 v_corridor((char **) map, rooms[i - 1].cy, rooms[i].cy, rooms[i - 1].cx);
    //                 h_corridor((char **) map, rooms[i - 1].cx, rooms[i].cx, rooms[i].cy);
    //             }
    //         }
    //     }
    // }
    
    // // Place player and stairs
    // int n = rand(0, room_count);
    // *player_x = rooms[n].cx;
    // *player_y = rooms[n].cy;

    // // Clear map
    // for (int i = 0; i < room_count; i++) {
    //     for (int x = rooms[i].x1; x < rooms[i].x2; x++) {
    //         for (int y = rooms[i].y1; y < rooms[i].y2; y++) {
    //             map[x][y] = 0;
    //         }
    //     }
    //     rooms[i].x1 = 0;
    //     rooms[i].x2 = 0;
    //     rooms[i].y1 = 0;
    //     rooms[i].y2 = 0;
    //     rooms[i].cx = 0;
    //     rooms[i].cy = 0;
    // }
}

int intersect(room r1, room r2) {
    return (r1.x1 <= r2.x2) && (r1.x2 >= r2.x1) && (r1.y1 <= r2.y2) && (r1.y2 >= r2.y1);
}

void h_corridor(char **map, int x1, int x2, int y) {
    int a, b;
    if (x1 <= x2) {
        a = x1;
        b = x2;
    } else {
        b = x1;
        a = x2;
    }
    for (int x = a; x <= b; x++) {
        map[x][y] = 0;
    }
}

void v_corridor(char **map, int y1, int y2, int x) {
    int a, b;
    if (y1 <= y2) {
        a = y1;
        b = y2;
    } else {
        b = y1;
        a = y2;
    }
    for (int y = a; y <= b; y++) {
        map[x][y] = 0;
    }
}

int is_valid(int x, int y) {
    return 0 <= x && x < MAP_WIDTH && 0 <= y && y < MAP_HEIGHT;
}