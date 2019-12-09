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
            if(tile->x == 0 && tile->y == 0) tile->player = true;
            else tile->player = false;

            if(tile->color == 0) tile->color++;

            game->color_map[x][y] = *tile;

        }

    }

    game->state = 0;

    if(subscribe(game) != 0) return NULL;

    //GAME LOGO
    draw_xpm(game_logo, 350, 100);

    //GAME OPTIONS

    draw_xpm(x1, 390, 300);
    draw_xpm(startG, 460, 305);
    
    draw_xpm(x2, 390, 400);
    draw_xpm(out, 445, 400);

    return game;

}

void draw_game(Game *game){

    if(game->state == 1) draw_xpm(game_logo, 350, 5);

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
            for(int i = 0; i < ROW; i++){
                for(int j = 0; j < COL; j++){
                    if(game->color_map[j][i].player) game->color_map[j][i].color = 1;
                }
            }
            draw_game(game);
            break;
        case GREEN:
            for(int i = 0; i < ROW; i++){
                for(int j = 0; j < COL; j++){
                    if(game->color_map[j][i].player) game->color_map[j][i].color = 2;
                }
            }
            draw_game(game);
            break;
        case CYAN:
            for(int i = 0; i < ROW; i++){
                for(int j = 0; j < COL; j++){
                    if(game->color_map[j][i].player) game->color_map[j][i].color = 3;
                }
            }
            draw_game(game);
            break;
        case RED:
            for(int i = 0; i < ROW; i++){
                for(int j = 0; j < COL; j++){
                    if(game->color_map[j][i].player) game->color_map[j][i].color = 4;
                }
            }
            draw_game(game);
            break;
        case PURPLE:
            for(int i = 0; i < ROW; i++){
                for(int j = 0; j < COL; j++){
                    if(game->color_map[j][i].player) game->color_map[j][i].color = 5;
                }
            }
            draw_game(game);
            break;
        case YELLOW:
            for(int i = 0; i < ROW; i++){
                for(int j = 0; j < COL; j++){
                    if(game->color_map[j][i].player) game->color_map[j][i].color = 6;
                }
            }
            draw_game(game);
            break;
        default:
            break;
    
    }

    calculate_player_block(game);
}

void calculate_player_block(Game *game){
    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COL; j++){
            if(game->color_map[j][i].player){
                if(game->color_map[j-1][i].color == game->color_map[j][i].color && j-1 >= 0 && j-1 <= 9) game->color_map[j-1][i].player = true;
                if(game->color_map[j+1][i].color == game->color_map[j][i].color && j+1 >= 0 && j+1 <= 9) game->color_map[j+1][i].player = true;
                if(game->color_map[j][i-1].color == game->color_map[j][i].color && i-1 >= 0 && i-1 <= 9) game->color_map[j][i-1].player = true;
                if(game->color_map[j][i+1].color == game->color_map[j][i].color && i+1 >= 0 && i+1 <= 9) game->color_map[j][i+1].player = true;
            }
        }
    }
}

void game_finished(Game *game){
    for(int i = 0; i < ROW; i++){
        for(int j = 0; j < COL; j++){
            if(game->color_map[j][i].player == false) return;
        }
    }
    game->state = 2;
    sleep(2);
}

void play_game(Game *game){

    //CHECK FOR INTERRUPTS
    int ipc_status;
	message msg;
	int r;
    int counter_min = 0, counter_sec = 0, counter_sec_aux = 0;
    
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
                        if((global_timer_counter % 60 == 0) && game->state == 1){

                            counter_sec_aux++;
                            if(counter_sec_aux > 9){
                                counter_sec++;
                                counter_sec_aux = 0;
                            }
                            if(counter_sec == 6){
                                counter_sec = 0;
                                counter_min++;
                            }
                            
                            if(counter_sec_aux == 0) draw_xpm(x0, 950, 100);
                            if(counter_sec_aux == 1) draw_xpm(x1, 950, 100);
                            if(counter_sec_aux == 2) draw_xpm(x2, 950, 100);
                            if(counter_sec_aux == 3) draw_xpm(x3, 950, 100);
                            if(counter_sec_aux == 4) draw_xpm(x4, 950, 100);
                            if(counter_sec_aux == 5) draw_xpm(x5, 950, 100);
                            if(counter_sec_aux == 6) draw_xpm(x6, 950, 100);
                            if(counter_sec_aux == 7) draw_xpm(x7, 950, 100);
                            if(counter_sec_aux == 8) draw_xpm(x8, 950, 100);
                            if(counter_sec_aux == 9) draw_xpm(x9, 950, 100);

                            if(counter_sec == 0) draw_xpm(x0, 900, 100);
                            if(counter_sec == 1) draw_xpm(x1, 900, 100);
                            if(counter_sec == 2) draw_xpm(x2, 900, 100);
                            if(counter_sec == 3) draw_xpm(x3, 900, 100);
                            if(counter_sec == 4) draw_xpm(x4, 900, 100);
                            if(counter_sec == 5) draw_xpm(x5, 900, 100);

                            if(counter_min == 0) draw_xpm(x0, 825, 100);
                            if(counter_min == 1) draw_xpm(x1, 825, 100);
                            if(counter_min == 2) draw_xpm(x2, 825, 100);
                            if(counter_min == 3) draw_xpm(x3, 825, 100);
                            if(counter_min == 4) draw_xpm(x4, 825, 100);
                            if(counter_min == 5) draw_xpm(x5, 825, 100);
                            if(counter_min == 6) draw_xpm(x6, 825, 100);
                            if(counter_min == 7) draw_xpm(x7, 825, 100);
                            if(counter_min == 8) draw_xpm(x8, 825, 100);
                            if(counter_min == 9) draw_xpm(x9, 825, 100);
                            
                        }
                    }

                    //KEYBOARD INTERRUPT
                    if (msg.m_notify.interrupts & game->irq_kbd) {
                        kbc_ih();

                        if(game->state == 0){
                            if(value == START_GAME){
                                game->state = 1;
                                clear_screen();
                                draw_game(game);
                            }else if(value == EXIT_GAME){
                                game->state = 2;
                                break;
                            }
                        } else if(game->state == 1){
                            handle_key(game);
                        }

                    }
                    game_finished(game);
                    break;
                default:
                    break;
            }
        } else {
          //Do nothing.
        }  
    }

}
