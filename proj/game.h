#include <lcom/lcf.h>
#include "graphics.h"
#include "keyboard.h"

#define MAX_NUMBER_TILES 100
#define COL 10
#define ROW 10
#define HEIGHT 55
#define WIDTH 55
#define DISPLACEMENT_X 230
#define DISPLACEMENT_Y 60
#define PLAYER_OPTIONS 6

struct tile {
    int x;
    int y;
    int width;
    int height;
    int color;
};
typedef struct tile Tile;

struct game {
    Tile color_map[COL][ROW];
    Tile player_block[MAX_NUMBER_TILES];

    //Subscribe interrupts
    int irq_timer, irq_kbd;
};
typedef struct game Game;

int subscribe(Game *game);

int unsubscribe();

Game* start_game();

void play_game(Game *game);

void draw_game(Game *game);

