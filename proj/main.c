#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

#include "game.h"

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/proj/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/proj/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (proj_main_loop)(int argc, char *argv[]) {

  if (vg_init(0x105) == NULL) {
    if(vg_exit() != 0) return 1;
    return 1;
  }

  Game *game = start_game();
  play_game(game);

  //CLEAR BUFFERS
  timer_int_handler();
  kbc_ih();
  mouse_ih();

  //UNSUBSCRIBE DEVICES
  if(unsubscribe() != 0) return 1;

  if(vg_exit() != 0) return 1;

  return 0;
}
