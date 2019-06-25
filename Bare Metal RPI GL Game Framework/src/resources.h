#ifndef RESOURCES_H
#define RESOURCES_H

unsigned short snickers[200][165];

unsigned short *get_player(int n);

unsigned short *get_enemy(int n, int type);

unsigned short *get_wall_tile(int n);

unsigned short *get_floor_tile(int n);

unsigned char demo_hardcode(int frame);

int get_font_bit(char letter, int r, int c);

#endif