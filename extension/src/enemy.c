#include "enemy.h"

void spawn_enemies(unsigned long long *enemies, uint8_t *map, int player_x, int player_y, int flr, int enemy_count) {
    for (int i = 0; i < enemy_count; i++) {
        enemy_t *e = (enemy_t *) &*(enemies + i);
        while (1) {
            e->x = get_random(0, MAP_WIDTH);
            e->y = get_random(0, MAP_HEIGHT);
            if (is_movable(map, e->x, e->y) &&
                (((player_x - e->x) * (player_x - e->x)
                + (player_y - e->y) * (player_y - e->y)) > 50)) {
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

void make_move(enemy_t *enemy, uint8_t *map, int player_x, int player_y, int player_moving, int player_dir) {
    int new_player_x = player_x;
    int new_player_y = player_y;
    if (player_dir == 0) {
        new_player_x--;
    } else if (player_dir == 1) {
        new_player_x++;
    } else if (player_dir == 2) {
        new_player_y--;
    } else if (player_dir == 3) {
        new_player_y++;
    }
    int dist_sq = calc_dist_sq(player_x, player_y, enemy->x, enemy->y);
    int valid_move_directions[4] = { -1, -1, -1, -1 };
    int valid_moves = 0;
    if (is_movable(map, enemy->x - 1, enemy->y)) {
        valid_move_directions[0] = 0;
        valid_moves++;
    }
    if (is_movable(map, enemy->x + 1, enemy->y)) {
        valid_move_directions[1] = 1;
        valid_moves++;
    }
    if (is_movable(map, enemy->x, enemy->y - 1)) {
        valid_move_directions[2] = 2;
        valid_moves++;
    }
    if (is_movable(map, enemy->x, enemy->y + 1)) {
        valid_move_directions[3] = 3;
        valid_moves++;
    }
    if (dist_sq <= enemy->line_of_sight * enemy->line_of_sight) {
        if (player_moving) {
            if (are_adjacent(player_x, player_y, enemy->x, enemy->y)) {
                if (valid_move_directions[player_dir] != -1) {
                    move(enemy, player_dir, map);
                } else if ((player_x < enemy->x) && (valid_move_directions[0] != -1)) {
                    move(enemy, 0, map);
                } else if ((player_x > enemy->x) && (valid_move_directions[1] != -1)) {
                    move(enemy, 1, map);
                } else if ((player_y < enemy->y) && (valid_move_directions[2] != -1)) {
                    move(enemy, 2, map);
                } else if ((player_y > enemy->y) && (valid_move_directions[3] != -1)) {
                    move(enemy, 3, map);
                }
            } else if (!are_adjacent(enemy->x, enemy->y, new_player_x, new_player_y)) {
                move_toward(enemy, new_player_x, new_player_y, valid_move_directions, map);
            }
        } else if (are_adjacent(player_x, player_y, enemy->x, enemy->y)) {
            if (player_x < enemy->x) {
                set_direction(enemy, 0);
            } else if (player_x > enemy->x) {
                set_direction(enemy, 1);
            } else if (player_y < enemy->y) {
                set_direction(enemy, 2);
            } else if (player_y > enemy->y) {
                set_direction(enemy, 3);
            }
            set_attacking(enemy, 1);
        } else {
            move_toward(enemy, new_player_x, new_player_y, valid_move_directions, map);
        }
    } else {
        if (valid_moves > 0) {
            int dir = get_random(0, 4);
            while (valid_move_directions[dir] == -1) {
                dir = get_random(0, 4);
            }
            move(enemy, dir, map);
        }
    }
}

void move_toward(enemy_t *enemy, int target_x, int target_y, int valid_move_directions[4], uint8_t *map) {
    int dir = -1;
    int min = calc_dist_sq(target_x, target_y, enemy->x, enemy->y);
    int nx = enemy->x, ny = enemy->y, dq;
    for (int i = 0; i < 4; i++) {
        if (valid_move_directions[i] != -1) {
            if (i == 0) {
                nx--;
            } else if (i == 1) {
                nx++;
            } else if (i == 2) {
                ny--;
            } else {
                ny++;
            }
            dq = calc_dist_sq(nx, ny, target_x, target_y);
            if (dq < min) {
                min = dq;
                dir = i;
            }
            
        }
    }
    if (dir != -1) {
        move(enemy, dir, map);
    }
}

void move(enemy_t *enemy, int dir, uint8_t* map) {
    set_moving(enemy, 1);
    set_direction(enemy, dir);
    int ex = enemy->x, ey = enemy->y;
    *(map + ex * MAP_HEIGHT + ey) &= 0xEF;
    if (dir == 0) {
        ex--;
    } else if (dir == 1) {
        ex++;
    } else if (dir == 2) {
        ey--;
    } else if (dir == 3) {
        ey++;
    }
    *(map + ex * MAP_HEIGHT + ey) |= 1 << 4;
}

void end_turn(enemy_t *enemy) {
    int dir = get_direction(enemy);
    if (dir == 0) {
        enemy->x--;
    } else if (dir == 1) {
        enemy->x++;
    } else if (dir == 2) {
        enemy->y--;
    } else if (dir == 3) {
        enemy->y++;
    }
    set_attacking(enemy, 0);
    set_moving(enemy, 0);
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
    if (!((direction >> 1) & 1)) {
        enemy->animation &= 0xEF | ((direction == 0) << 4);
    }
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