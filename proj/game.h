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
#define START_GAME 0x02
#define EXIT_GAME 0x03

//KEYS FOR THE PLAYER
#define BLUE 0x02
#define GREEN 0x03
#define CYAN 0x04
#define RED 0x05
#define PURPLE 0x06
#define YELLOW 0x07

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
    int size;

    //Subscribe interrupts
    int irq_timer, irq_kbd; 
    int state; //State 0 MENU, state 1 PLAY GAME, state 2 EXIT GAME
};
typedef struct game Game;

int subscribe(Game *game);

int unsubscribe();

Game* start_game();

void play_game(Game *game);

void draw_game(Game *game);

void handle_key(Game *game);
