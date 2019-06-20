#ifndef ENEMY_H
#define ENEMY_H

#include <stdint.h>
#include "map_generator.h"

#define MAX_ENEMIES 20
#define ENEMY_TYPES 1

typedef struct enemy {
    uint8_t type;
    uint8_t x;
    uint8_t y;
    uint8_t hp;
    uint8_t atk;
    uint8_t acc;
    uint8_t line_of_sight;
    uint8_t animation; // facing_left | direction | direction | moving | attacking
} enemy_t;

void spawn_enemies(unsigned long long *enemies, uint8_t *map, int player_x, int player_y, int flr, int enemy_count);

int is_attacking(enemy_t *enemy);

int is_moving(enemy_t *enemy);

int get_direction(enemy_t *enemy);

int is_facing_left(enemy_t *enemy);

void set_attacking(enemy_t *enemy, uint8_t attacking);

void set_moving(enemy_t *enemy, uint8_t moving);

void set_direction(enemy_t *enemy, uint8_t direction);

int get_attack(enemy_t *enemy, int lck);

void damage(enemy_t *enemy, int dmg);

int getXP(enemy_t *enemy, int flr);

int powow(int x, int n);

#endif