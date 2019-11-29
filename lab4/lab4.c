// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include "utils.h"
#include "mouse.h"

// Any header files included below this line should have been created by you

uint8_t mouse_scancodes[3];
extern int ps2_hook_id;

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {
    uint8_t hook_id, r;
    int ipc_status, irq_set = 0;
    uint32_t counter = 0;
    message msg;
    int index = 0;

    if(ps2_subscribe_int(&hook_id) == 0){
        irq_set = BIT(hook_id);

        mouse_enable_disable_data(ENABLE);

        while( counter < cnt ) {

            if((r = driver_receive(ANY, &msg, &ipc_status)) != 0){

                printf("Driver_receive failed with : %d", r);
                continue;

            }

            if(is_ipc_notify(ipc_status)){
                switch(_ENDPOINT_P(msg.m_source)){
                    case HARDWARE:
                        if(msg.m_notify.interrupts & irq_set){

                            mouse_ih();

                            mouse_scancodes[index] = value;

                            index++;

                            if(index == 3){

                            /*
                                    struct packet {
                                        uint8_t bytes[3]; // mouse packet raw bytes
                                        bool rb, mb, lb;  // right, middle and left mouse buttons pressed
                                        int16_t delta_x;  // mouse x-displacement: rightwards is positive
                                        int16_t delta_y;  // mouse y-displacement: upwards is positive
                                        bool x_ov, y_ov;  // mouse x-displacement and y-displacement overflows
                                    };
                                */

                                struct packet mouse_packet;
                                packet_parser(&mouse_packet, mouse_scancodes);

                                index = 0;
                                counter++;

                                mouse_print_packet(&mouse_packet);
                                

                            }

                        }
                        break;
                    default:
                        break;

                }
            }
            else{

            }
        }
        
        if(mouse_enable_disable_data(DISABLE) == 0){

            if(ps2_unsubscribe_int() == 0)
                return 0;
            else return 1;
        }
        return 1;

    }

    return 1;
}

int(mouse_test_remote)(uint16_t period, uint8_t cnt) {
        int s = 1;

    if(period < 0 || cnt < 1)
    {
        return 1;
    }

    while(cnt > 0)
    {
        if(issueMouseCommand(READ_DATA) != 0)
        {
            return 1;
        }

        for(unsigned int i = 0; i < 3;)
        {
            mouse_ih();

            if(SET_DEF == true)
            {
                continue;
            }

            if(s == 1)
            {
                uint8_t temp = (uint8_t)value;

                if(temp & BIT(3))
                {
                    mouse_scancodes[0] = value;
                    s++;
                    i++;
                }
                continue;
            }

            if(s == 2)
            {
                mouse_scancodes[1] = value;
                s++;
                i++;
                continue;
            }

            mouse_scancodes[2] = value;
            i++;
            s = 1;
        }

        struct packet mouse_packet;
        packet_parser(&mouse_packet, mouse_scancodes);
        mouse_print_packet(&mouse_packet);
        tickdelay(micros_to_ticks(period * 1000));

        cnt--;
    }

    return 0;

}



int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}



int(mouse_test_async)(uint8_t idle_time) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;}






