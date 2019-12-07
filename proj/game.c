#include <lcom/lcf.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "timer.c"

Game* start_game(){
    time_t t;
    srand((unsigned) time(&t));

    //Creating the game struct
    Game* game = (Game*) malloc(sizeof(Game));
    if(game == NULL) return NULL;

    //Initializing all the tiles of the game
    for(int x = 0; x < ROW; x++){

        for(int y = 0; y < COL; y++){

            Tile* tile = (Tile*) malloc(sizeof(Tile));
            tile->x = x * WIDTH;
            tile->y = y * HEIGHT;
            tile->color = rand() % 7;

            if(tile->color == 0) tile->color++;

            game->color_map[y][x] = *tile;

        }

    }

    //Initializing the player block, which always corresponds to position 0, 0
    Tile* player_tile = (Tile*) malloc(sizeof(Tile));
    if(player_tile == NULL) return NULL;

    player_tile->x = 0;
    player_tile->y = 0;
    player_tile->height = HEIGHT;
    player_tile->width = WIDTH;
    player_tile->color = rand() % 7;
    if(player_tile->color == 0) player_tile->color++;

    //Putting in the game player_block the player_tile
    game->player_block[0] = *player_tile;

    game->state = 0;

    game->size = 1;

    if(subscribe(game) != 0) return NULL;

    return game;

}

void draw_game(Game *game){

    uint16_t height = (uint16_t) HEIGHT, width = (uint16_t) WIDTH;

    for(int i = 0; i < ROW; i++){

        for(int j = 0; j < COL; j++){

            uint16_t x = game->color_map[j][i].x + DISPLACEMENT_X;
            uint16_t y = game->color_map[j][i].y + DISPLACEMENT_Y;
            uint32_t color = game->color_map[j][i].color;

            vg_draw_rectangle(x, y, width, height, color);

        }

    }

    uint16_t x = DISPLACEMENT_X;
    uint16_t y = game->color_map[9][9].y + 150;

    for(int i = 0; i < PLAYER_OPTIONS; i++){

        vg_draw_rectangle(x + (width * i) + 44 * i, y, width, height, (uint32_t) i+1);
    }

}

int subscribe(Game *game){
    uint8_t hook_timer, hook_kbd;

    if(timer_subscribe_int(&hook_timer) != 0) return -1;
    if(kbc_subscribe_int(&hook_kbd) != 0) return -1;

    game->irq_timer = BIT(hook_timer);
    game->irq_kbd = BIT(hook_kbd);

    return 0;
    
}

int unsubscribe(){

    if(timer_unsubscribe_int() != 0) return -1;
    if(kbc_unsubscribe_int() != 0) return -1;

    return 0;
    
}

void handle_key(Game *game){
    switch(value){
        case BLUE:

            for(int i = 0; i < game->size; i++){

                if(game->player_block[i].color == 1) continue;

                game->player_block[i].color = 1;
                game->color_map[game->player_block[i].x][game->player_block[i].y].color = game->player_block[i].color;
            }

            draw_game(game);
            break;
        case GREEN:

            for(int i = 0; i < game->size; i++){

                if(game->player_block[i].color == 2) continue;

                game->player_block[i].color = 2;
                game->color_map[game->player_block[i].x][game->player_block[i].y].color = game->player_block[i].color;
            }

            draw_game(game);
            break;

        case CYAN:

            for(int i = 0; i < game->size; i++){

                if(game->player_block[i].color == 3) continue;

                game->player_block[i].color = 3;
                game->color_map[game->player_block[i].x][game->player_block[i].y].color = game->player_block[i].color;
            }

            draw_game(game);
            break;

        case RED:

            for(int i = 0; i < game->size; i++){

                if(game->player_block[i].color == 4) continue;

                game->player_block[i].color = 4;
                game->color_map[game->player_block[i].x][game->player_block[i].y].color = game->player_block[i].color;
            }

            draw_game(game);
            break;

        case PURPLE:

            for(int i = 0; i < game->size; i++){

                if(game->player_block[i].color == 5) continue;

                game->player_block[i].color = 5;
                game->color_map[game->player_block[i].x][game->player_block[i].y].color = game->player_block[i].color;
            }

            draw_game(game);
            break;

        case YELLOW:

            for(int i = 0; i < game->size; i++){

                if(game->player_block[i].color == 6) continue;

                game->player_block[i].color = 6;
                game->color_map[game->player_block[i].x][game->player_block[i].y].color = game->player_block[i].color;
            }

            draw_game(game);
            break;

        default:
            break;
    
    }
}

void play_game(Game *game){

    //COLOCAR AQUI FUNÇÃO QUE DÁ DRAW DAS SPRITES:
    //  1. LOGO DO JOGO
    //  2. START GAME
    //  3. EXIT GAME

    //CHECK FOR INTERRUPTS
    int ipc_status;
	message msg;
	int r;
    
    //MIGHT CHANGE THE CONDITION
    while (value != ESC_BREAK_CODE && game->state != 2) {

        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }
    
        if (is_ipc_notify(ipc_status)) {

            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE:

                    //TIMER INTERRUPT
                    if (msg.m_notify.interrupts & game->irq_timer) {
                        timer_int_handler();
                        if(global_timer_counter % 60 == 0){
                            printf("1 SEGUNDO PASSOU\n");
                        }
                    }

                    //KEYBOARD INTERRUPT
                    if (msg.m_notify.interrupts & game->irq_kbd) {
                        kbc_ih();

                        if(game->state == 0){
                            if(value == START_GAME){
                                game->state = 1;
                                draw_game(game);
                            }else if(value == EXIT_GAME){
                                game->state = 2;
                                break;
                            }
                        } else if(game->state == 1){
                            handle_key(game);
                        }
                    }
                    break;
                default:
                    break;
            }
        } else {
          //Do nothing.
        }  
    
    }

}
