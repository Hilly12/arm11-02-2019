#include "enemy.h"

void spawn_enemies(unsigned long long *enemies, uint8_t *map, int player_x, int player_y, int flr, int enemy_count) {
    for (int i = 0; i < enemy_count; i++) {
        enemy_t *e = (enemy_t *) &*(enemies + i);
        while (1) {
            e->x = get_random(0, MAP_WIDTH);
            e->y = get_random(0, MAP_HEIGHT);
            if ((*(map + e->x * MAP_HEIGHT + e->y) & 1) 
                && (((player_x - e->x) * (player_x - e->x)
                + (player_y - e->y) * (player_y - e->y)) > 100)) {
                break;
            }
        }
        int lvl = (flr / 14) + (get_random(0, 200) - 100) * (get_random(0, 200) - 100) * 3 / 20000;
        while ((lvl < 0) || (lvl > 7)) {
            lvl = (flr / 14) + (get_random(0, 200) - 100) * (get_random(0, 200) - 100) * 3 / 20000;
        }
        e->type = get_random(0, ENEMY_TYPES);
        e->animation = 1 << 2; // face right
        e->hp = 10 * powow(3, lvl) / powow(2, lvl);
        e->atk = 10 * powow(6, lvl) / powow(5, lvl);
        e->acc = 80 + 2 * lvl;
        e->line_of_sight = 5 + lvl;
    }
}

int is_attacking(enemy_t *enemy) {
    return enemy->animation & 1;
}

int is_moving(enemy_t *enemy) {
    return (enemy->animation >> 1) & 1;
}

int get_direction(enemy_t *enemy) {
    return (enemy->animation >> 2) & 2;
}

int is_facing_left(enemy_t *enemy) {
    return (enemy->animation >> 4) & 1;
}

void set_attacking(enemy_t *enemy, uint8_t attacking) {
    enemy->animation &= 0xFE | (attacking & 1);
}

void set_moving(enemy_t *enemy, uint8_t moving) {
    enemy->animation &= 0xFD | ((moving & 1) << 1);
}

void set_direction(enemy_t *enemy, uint8_t direction) {
    enemy->animation &= 0xF3 | ((direction & 2) << 2);
}

int get_attack(enemy_t *enemy, int lck) {
    return (get_random(0, 100) <= (enemy->acc * (100 - lck / 2) / 100)) ? (enemy->atk + get_random(0, 3) - 1) : 0;
}

void damage(enemy_t *enemy, int dmg) {
    enemy->hp -= dmg;
}

int getXP(enemy_t *enemy, int flr) {
    int lvl = (flr / 14) + (get_random(0, 200) - 100) * (get_random(0, 200) - 100) * 3 / 20000;
    return 50 + 10 * powow(7, lvl) / powow(2, lvl);
}

int powow(int x, int n) {
    int res = 1;
    for (int i = 0; i < n; i++) {
        res *= x;
    }
    return res;
}