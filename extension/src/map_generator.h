#define MAP_WIDTH 128
#define MAP_HEIGHT 72
#define VISIBLE_WIDTH 16
#define VISIBLE_HEIGHT 12
#define SQUARE_SIZE (WIDTH / VISIBLE_WIDTH)
#define HCENT 7
#define VCENT 5

typedef struct room {
    int x1;
    int x2;
    int y1;
    int y2;
    int cx;
    int cy;
} room;

void generate_map(char **map, int floor, int *player_x, int *player_y);

int intersect(room r1, room r2);

void h_corridor(char **map, int x1, int x2, int y);

void v_corridor(char **map, int y1, int y2, int x);

int is_valid(int x, int y);