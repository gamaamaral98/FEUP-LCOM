// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

#include "graphics.h"
#include "keyboard.h"
#include "timer.c"

extern int global_timer_counter;

// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab5/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay) {
  /* To be completed */
  printf("%s(0x%03x, %u): under construction\n", __func__, mode, delay);

    if(set_graph(mode) != 0) return 1;
    
    sleep(delay);
    
    if(vg_exit() == 0) return 0;

    return 1;

}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color) {
  /* To be completed */
  printf("%s(0x%03X, %u, %u, %u, %u, 0x%08x): under construction\n",
         __func__, mode, x, y, width, height, color);

  uint8_t hook_id, r;
  int ipc_status;
  message msg;

  if(vg_init(mode) != NULL){ 

    height = handle_height(x, y, height);
    width = handle_width(x, y, width);
    vg_draw_rectangle(x,y,width,height,color);
  
  }else return 1;

  if(kbc_subscribe_int(&hook_id) == 0){

    r = BIT(hook_id);

      while(value!= ESC_BREAK_CODE) {
    
        if((hook_id = driver_receive(ANY, &msg, &ipc_status)) != 0){
    
          printf("Driver_receive failed with : %d", hook_id);
          continue;
    
        }
    
        if(is_ipc_notify(ipc_status)){
    
          switch(_ENDPOINT_P(msg.m_source)){
    
            case HARDWARE:
              if(msg.m_notify.interrupts & r){
    
                kbc_ih();
              
              }
              break;
            default:
              break;
          
          }
        
        } else {
          //Do nothing.
        
        }
      
      }
      
      if(kbc_unsubscribe_int() != 0) return 1;

      if(vg_exit() == 0) return 0;
      else return 1;
  
  }
  
  return 1;

}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step) {
  /* To be completed */
  printf("%s(0x%03x, %u, 0x%08x, %d): under construction\n", __func__,
         mode, no_rectangles, first, step);

  uint8_t hook_id, r;
  int ipc_status;
  message msg;

  if(vg_init(mode) != NULL) vg_draw_pattern(mode, no_rectangles, first, step);
  else return 1;

  if(kbc_subscribe_int(&hook_id) == 0){

    r = BIT(hook_id);

      while(value!= ESC_BREAK_CODE) {
    
        if((hook_id = driver_receive(ANY, &msg, &ipc_status)) != 0){
    
          printf("Driver_receive failed with : %d", hook_id);
          continue;
    
        }
    
        if(is_ipc_notify(ipc_status)){
    
          switch(_ENDPOINT_P(msg.m_source)){
    
            case HARDWARE:
              if(msg.m_notify.interrupts & r){
    
                kbc_ih();
              
              }
              break;
            default:
              break;
          
          }
        
        } else {
          //Do nothing.
        
        }
      
      }
      
      if(kbc_unsubscribe_int() != 0) return 1;

      if(vg_exit() == 0) return 0;
      else return 1;
  
  }
  
  return 1;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y) {
  /* To be completed */
  printf("%s(%8p, %u, %u): under construction\n", __func__, xpm, x, y);

  uint8_t hook_id, r;
  int ipc_status;
  message msg;

  if(vg_init(MODE) != NULL){

    draw_xpm(xpm, x, y);

  }

  else return 1;

  if(kbc_subscribe_int(&hook_id) == 0){

    r = BIT(hook_id);

      while(value!= ESC_BREAK_CODE) {
    
        if((hook_id = driver_receive(ANY, &msg, &ipc_status)) != 0){
    
          printf("Driver_receive failed with : %d", hook_id);
          continue;
    
        }
    
        if(is_ipc_notify(ipc_status)){
    
          switch(_ENDPOINT_P(msg.m_source)){
    
            case HARDWARE:
              if(msg.m_notify.interrupts & r){
    
                kbc_ih();
              
              }
              break;
            default:
              break;
          
          }
        
        } else {
          //Do nothing.
        
        }
      
      }
      
      if(kbc_unsubscribe_int() != 0) return 1;

      if(vg_exit() == 0) return 0;
      else return 1;
  
  }
  
  return 1;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate) {
  /* To be completed */
  printf("%s(%8p, %u, %u, %u, %u, %d, %u): under construction\n",
         __func__, xpm, xi, yi, xf, yf, speed, fr_rate);

  uint8_t hook_id, kbc_hook_id;
  int ipc_status;
  message msg;
  int r, flag, distance, count = 0; //0 for HORIZONTAL 1 for VERTICAL

  if(xf == xi && yf == yi) return 1;

  //Check if the movement is horizontal or vertical

  if(xf - xi == 0 && yf - yi != 0){ flag = 1; distance = yf-yi; }
  if(xf - xi != 0 && yf - yi == 0){ flag = 0; distance = xf-xi; }

  vg_init(MODE);
  draw_xpm(xpm, xi, yi);

  if(kbc_subscribe_int(&kbc_hook_id) == 0){

    if(timer_subscribe_int(&hook_id)!=0) return 1;

    r = BIT(kbc_hook_id);

    while(value != ESC_BREAK_CODE){

      if((hook_id = driver_receive(ANY, &msg, &ipc_status)) != 0){

        printf("Driver_receive failed with : %d", hook_id);
        continue;

      }

      if(is_ipc_notify(ipc_status)) {

        switch(_ENDPOINT_P(msg.m_source)){

          case HARDWARE:

            if(msg.m_notify.interrupts & r){

              kbc_ih();

            }

            if(msg.m_notify.interrupts & 1){

              timer_int_handler();

              if((global_timer_counter % (60/fr_rate) == 0) && (count < distance)){
                //timer_print_elapsed_time();

                //horizontal 
                if(flag == 0){
                  xi += speed;
                }
                //vertical
                if(flag == 1){
                  yi += speed;
                }
                clear_screen();
                draw_xpm(xpm, xi, yi);
                count += speed;
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

    if(kbc_unsubscribe_int() != 0) return 1;

    if(timer_unsubscribe_int() != 0) return 1;

    if(vg_exit() != 0){
      return 1;
    }

    return 0;

  }

  return 1;
}

int(video_test_controller)() {
  /* To be completed */
  printf("%s(): under construction\n", __func__);

  return 1;
}
