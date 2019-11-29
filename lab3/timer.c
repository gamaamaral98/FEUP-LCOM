#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int global_timer_counter = 0, hook_id;

int (timer_set_frequency)(uint8_t timer, uint32_t freq) {
  /* To be implemented by the students */
  if((timer < 0) || (timer > 2)) return 1;

  uint32_t read_back = TIMER_RB_COUNT_ | TIMER_RB_CMD | TIMER_RB_SEL(timer);

  if(sys_outb(TIMER_CTRL, read_back) == OK){

    uint8_t port = TIMER_0 + timer;
    uint8_t out_read_back;

    if(util_sys_inb(port, &out_read_back) == OK){

      //Control word
      uint8_t control_word = (uint8_t)out_read_back;
      control_word = (control_word & 0x0F) | TIMER_LSB_MSB;

      switch(timer){

        case 0:
          control_word = control_word | TIMER_SEL0;
          break;
        case 1:
          control_word = control_word | TIMER_SEL1;
          break;
        case 2:
          control_word = control_word | TIMER_SEL2;
          break;
        default:
          return 1;

      }

      if(sys_outb(TIMER_CTRL, control_word) == OK){

        if(freq <= 18) return 1;
        uint16_t timer_value = (uint16_t)(TIMER_FREQ / freq);
        uint8_t timer_lsb;
        util_get_LSB(timer_value, &timer_lsb);

        if(sys_outb(port, timer_lsb) == OK){

          uint8_t timer_msb;
          util_get_MSB(timer_value, &timer_msb);

          if(sys_outb(port, timer_msb) == OK){

            return 0;

          }

          return 1;

        }

        return 1;

      }

      return 1;

    }

    return 1;
  
  }

  return 1;

}

int (timer_subscribe_int)(uint8_t *bit_no) {
    /* To be implemented by the students */
  if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) == 0){
    
    *bit_no = hook_id;
    return 0;
  
  }

  printf("Error while subscribing!\n");

  return 1;
}

int (timer_unsubscribe_int)() {
  /* To be implemented by the students */
  if(sys_irqrmpolicy(&hook_id) == 0) return 0;

  printf("Error while unsubscribing!\n");

  return 1;

}

void (timer_int_handler)() {
  /* To be implemented by the students */
  global_timer_counter++; 
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {
  /* To be implemented by the students */
  if((timer < 0) || (timer > 2)) return 1;

  uint32_t read_back = TIMER_RB_COUNT_ | TIMER_RB_CMD | TIMER_RB_SEL(timer);

  if(sys_outb(TIMER_CTRL, read_back) == OK){

    if(util_sys_inb(TIMER_0 + timer, st) == 0){

      return 0;

    }

  }

  return 1;
}

int (timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field) {
  /* To be implemented by the students */
  if((timer < 0) || (timer > 2)) return 1;

  union timer_status_field_val status;
  switch(field) {
    case tsf_all:
      status.byte = st;
      break;
    case tsf_initial:
      if(st & TIMER_MSB){

        if(!(st & TIMER_LSB)) status.in_mode = MSB_only;
        else status.in_mode = MSB_after_LSB;

        break;

      }

      if(st & TIMER_LSB){

        status.in_mode = LSB_only;

        break;

      }

      if(!(st & TIMER_LSB) && !(st & TIMER_MSB)){

        status.in_mode = INVAL_val;

        break;

      }

      break;
    case tsf_mode:
      if(!(st & TIMER_RATE_GEN)){

        if(!(st & (0x1 << 3))){

          if(st & (0x1 << 1)) {

            status.count_mode = 1;
            break;

          }

        } else {

          if(!(st & (0x1 << 1))){

            status.count_mode = 4;
            break;

          } 
          else{

            status.count_mode = 5;
            break;

          } 

        }

      } else {

        if(!(st & (0x1 << 1))){

          status.count_mode = 2;
          break;

        } else{

          status.count_mode = 3;
          break;

        }

      }

      break;

    case tsf_base:
      if(st & TIMER_BCD) status.bcd = true;
      else status.bcd = false;
      break;
    default:
      return 1;
  }

  if(timer_print_config(timer, field, status) != 0) return 1;

  return 0;
}
