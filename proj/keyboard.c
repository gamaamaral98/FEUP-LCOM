#include <lcom/lcf.h>
#include <keyboard.h>
#include "utils.h"

int kbc_hook_id = 2;
uint32_t value = 0;

int sys_inb_count(int port, uint8_t *value) {
   
    return util_sys_inb(port, value);

}

void (handle_code)(bool *make, uint32_t value) {

    if(value & BIT(7)) *make = false;
    else *make = true;

}

int (kbc_subscribe_int)(uint8_t *bit_no) {

    *bit_no = kbc_hook_id;

    if(sys_irqsetpolicy(KEYBOARD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &kbc_hook_id) == 0) return 0;

    return 1;

}

int (kbc_unsubscribe_int)() {

    if(sys_irqrmpolicy(&kbc_hook_id) == 0) return 0;

    return 1;

}

void (kbc_ih)() {
    uint8_t status;
    int tries = 0;
   
    while( 1 ) {
   
        if(tries >= 5) break;
   
        sys_inb_count(STAT_REG, &status);
   
        if( status & KBC_OBF ) {
   
            if(sys_inb(OUT_BUF, &value) == 0){
                        
                count++;

                if ((status & (PAR_ERR | TO_ERR)) == 0) break;

            } else break;
   
        }
   
        tries++;
   
    }
   
}

void (kbc_poll)() {
    uint8_t status;
    int tries = 0;
   
    while(tries < 5) {
   
        sys_inb_count(STAT_REG, &status); /* assuming it returns OK */
        /* loop while 8042 output buffer is empty */
   
        if( status & KBC_OBF ) {
   
            if(sys_inb(OUT_BUF, &value) == 0){
                        
                count++;

                if ((status & (PAR_ERR | TO_ERR)) == 0) break;

            } else break;
   
        }
      
        tries++;
   
    }
   
}
