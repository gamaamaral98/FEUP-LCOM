#include <lcom/lcf.h>

//To use tickdelay
#include <minix/sysutil.h>

//Usefull macros
#define VALUE 0xe0 
#define DELAY_US    20000
#define ESC_BREAK_CODE  0x81
#define OUT_BUF 0x60
#define STAT_REG 0x64
#define KBC_CMD_REG 0x64  
#define BREAK_CODE BIT(7)  

//STATUS REGISTER
#define PAR_ERR BIT(7)
#define TO_ERR  BIT(6)
#define KBC_AUX BIT(5)
#define KBC_INH BIT(4)
#define KBC_A2 BIT(3)
#define KBC_SYS BIT(2)
#define KBC_IBF BIT(1)
#define KBC_OBF BIT(0)

extern uint32_t value;

extern uint8_t c; // variavel global para salvar scancodes

int sys_inb_count(int port, uint8_t *value);

int (kbc_subscribe_int)(uint8_t *bit_no);

int (kbc_unsubscribe_int)();

int (kbc_read_value)();

void (handle_code)(bool *make, uint32_t value);

void (kbc_ih)();

void (kbc_poll)();
