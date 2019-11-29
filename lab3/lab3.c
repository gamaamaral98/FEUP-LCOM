#include <lcom/lcf.h>
#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>
#include "keyboard.h"
#include "utils.h"

extern int global_timer_counter;
extern bool err;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");
  
  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");
  
  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;
  
  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();
  
  return 0;
}

int(kbd_test_scan)() {
  uint8_t hook_id, scan_codes[2], index = 0, r;
  int ipc_status;
  count=0;
  value=0;
  message msg;
  bool make=false;
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
    
                scan_codes[index] = value;
    
                index++;
    
                if(value != VALUE) {
                
                  handle_code(&make, value);

                  if(kbd_print_scancode(make, index, scan_codes) == 0) index = 0;
                  else return 1;
               
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
      
      if(kbc_unsubscribe_int() == 0){
      
        if(kbd_print_no_sysinb(count) == 0) return 0;
        
        return 1;
      
      }
      
      return 1;
  
  }
  
  return 1;
 
}


int(kbd_test_poll)() {

    bool make = false; 
    uint8_t index = 1;
    
   	while(value != ESC_BREAK_CODE) {

      kbc_poll();
      
      handle_code(&make, value);

      uint8_t scan_code = (uint8_t) value;

      kbd_print_scancode(make, index, &scan_code);

      tickdelay(micros_to_ticks(DELAY_US));

    }  

    kbd_print_no_sysinb(count);

    return 0;

}



int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  uint8_t hook_id, irq_set, irq_timer0, scan_codes[2], index = 0, time_passed = 0;
  int ipc_status;
  count=0;
  value=0;
  message msg;
  bool make=false;


  if(timer_subscribe_int(&hook_id) == 0){

    irq_timer0 = BIT(hook_id);

    if(kbc_subscribe_int(&hook_id) == 0){

      irq_set = BIT(hook_id);

        while((time_passed < n) && (value != ESC_BREAK_CODE)) {
      
          if((hook_id = driver_receive(ANY, &msg, &ipc_status)) != 0){
      
            printf("Driver_receive failed with : %d", hook_id);
            continue;
      
          }
      
          if(is_ipc_notify(ipc_status)){
      
            switch(_ENDPOINT_P(msg.m_source)){
      
              case HARDWARE:

                if(msg.m_notify.interrupts & 1){

                  timer_int_handler();

                  if(global_timer_counter % 60 == 0){

                    time_passed++;

                  }

                }

                if(msg.m_notify.interrupts & irq_set){
      
                  kbc_ih();
      
                  scan_codes[index] = value;
      
                  index++;

                  if(value != VALUE) {

                    handle_code(&make, value);
                  
                    if(kbd_print_scancode(make, index, scan_codes) == 0){

                      index = 0;
                      time_passed = 0;

                    }
                    else return 1;
                
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

        if(kbd_print_no_sysinb(count) == 0){

          if(kbc_unsubscribe_int() == 0){

            if(timer_unsubscribe_int() == 0) return 0;

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
