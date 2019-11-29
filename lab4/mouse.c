#include <lcom/lcf.h>
#include "mouse.h"
#include "utils.h"

int ps2_hook_id = 1;
uint32_t value = 0;
bool err = true;
uint32_t stat;

int (ps2_subscribe_int)(uint8_t *bit_no) {

    *bit_no=ps2_hook_id ;

    if(sys_irqsetpolicy(KBD_AUX_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &ps2_hook_id) == 0)
    {

        return 0;}

    return 1;

}

int (mouse_enable_disable_data)(u32_t command) //DISABLE ENABLE
{

    sys_irqdisable(&ps2_hook_id);
    
    int i=0;
    
    while (i < 10) {
        u32_t status;

        //Write command 0xD4 (Write mouse byte) to port 0x64 (STAT_REG)
        sys_inb(STAT_REG, &status);
        if ((status & (AUX | IBF)) !=0) return 1; //Input buffer not ready and not reading from keyboard or mouse
        sys_outb(STAT_REG, WBM);
            

        //Write the desired command to port 0x60 
        sys_inb(STAT_REG, &status);
        if ((status & (AUX | IBF)) !=0) return 1;
        sys_outb(IN_BUF, command);
            

        //reads from the outbuf the status to check for an error or acknowledgement
        sys_inb(OUT_BUF, &status);
        if (status == ERROR)
            return 1;
        if (status == ACK){
            if(command == ENABLE) sys_irqenable(&ps2_hook_id);
            return 0;
        }
        i++;
    }
    return 1;
}

int (ps2_unsubscribe_int)() {

    if(sys_irqrmpolicy(&ps2_hook_id) == 0) return 0;

    return 1;

}

/*
    struct packet {
        uint8_t bytes[3]; // mouse packet raw bytes
        bool rb, mb, lb;  // right, middle and left mouse buttons pressed
        int16_t delta_x;  // mouse x-displacement: rightwards is positive
        int16_t delta_y;  // mouse y-displacement: upwards is positive
        bool x_ov, y_ov;  // mouse x-displacement and y-displacement overflows
    };
*/

void (packet_parser)(struct packet *mouse_packet, uint8_t scan_codes[]){

    mouse_packet->bytes[0] = scan_codes[0];
    mouse_packet->bytes[1] = scan_codes[1];
    mouse_packet->bytes[2] = scan_codes[2];

    mouse_packet->rb = scan_codes[0] & RB; 
    mouse_packet->mb = scan_codes[0] & MB;
    mouse_packet->lb = scan_codes[0] & LB;

    uint16_t dx = scan_codes[1], dy = scan_codes[2];

    if(scan_codes[0] & MSB_X_DELTA)  dx |= 0xFF00;
    else dx &= 0x00FF;

    if(scan_codes[0] & MSB_Y_DELTA)  dy |= 0xFF00;
    else dy &= 0x00FF;

    mouse_packet->delta_x = dx;
    mouse_packet->delta_y = dy;

    mouse_packet->x_ov = scan_codes[0] & X_OVFL;
    mouse_packet->y_ov = scan_codes[0] & Y_OVFL;

}

void (mouse_ih)() {
    uint8_t status;
   
    util_sys_inb(STAT_REG, &status); /* assuming it returns OK */
    /* loop while 8042 output buffer is empty */
   
    if( status & KBC_OBF ) {
   
        if(sys_inb(OUT_BUF, &value) == 0){

            if ((status & (PAR_ERR | TO_ERR)) != 0) err = true;

        } 
    
    }
   
}


int issueMouseCommand(uint8_t cmd) {
  uint32_t stat;
  uint32_t ack;
  do {
    sys_inb(KBC_CMD_REG, &stat);


    if (stat & IBF)
      continue;

    if (sys_outb(KBC_CMD_REG,   WBM) != OK)
      return 1;

    sys_inb(KBC_CMD_REG, &stat);


    if (stat & IBF)
      continue;

    if (sys_outb(OUT_BUF, cmd) != OK)
      return 1;

    sys_inb(KBC_CMD_REG, &stat);

    if ((stat & OUT_BUF) && (stat & AUX)) {
      if (sys_inb(OUT_BUF, &ack) != OK)
        return 1;

      if (ack == ERROR)
        return 1;
    }
  } while (ack != ACK);

  return 0;
}


uint8_t readMouseByte() {
    uint32_t stat;
    uint32_t data;

    do{

        sys_inb(KBC_CMD_REG, &stat);

        if ( (stat & AUX)&&(stat & OBF)) {
            sys_inb(OUT_BUF,&data);
            if ((stat & (PAR_ERR | TO_ERR)) == 0)
                return (uint8_t) data;
            else
                return MAIOR;
        }

        tickdelay(micros_to_ticks(DELAY_US));
    } while(1);
}


int defaultCommand() {
    uint8_t cmb = minix_get_dflt_kbc_cmd_byte();
    uint32_t stat;
    while (1) {
        sys_inb(KBC_CMD_REG, &stat);


        if (stat & IBF)
            continue;

        if (sys_outb(KBC_CMD_REG, WRITE_COMMAND_BYTE) != OK)
            return 1;

        sys_inb(KBC_CMD_REG, &stat);

        if (stat & IBF)
            continue;

        if (sys_outb(OUT_BUF, (uint32_t) cmb) != OK)
            return 1;

        break;
    }
    return 0;
}


int mouseIrqSet(bool enable) { //Function to enable and disable irq line
    if (enable) {
        if (sys_irqenable(&ps2_hook_id) != OK) //Enable irq line
            return 1;
    }
    else {
        if (sys_irqdisable(&ps2_hook_id) != OK) //Disable irq line
            return 1;
    }
    return 0;
}


