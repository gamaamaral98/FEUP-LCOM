#include <lcom/lcf.h>

//To use tickdelay
#include <minix/sysutil.h>

//Usefull macros
#define VALUE 0xe0 
#define DELAY_US    20000
#define ESC_BREAK_CODE  0x81
#define OUT_BUF 0x60
#define IN_BUF 0x60
#define STAT_REG 0x64
#define KBC_CMD_REG 0x64  
#define BREAK_CODE BIT(7)  
#define OBF 0x01
#define IBF 0x02
#define AUX 0x20
#define MAIOR -1
//MOUSE-RELATED KBC COMMANDS

#define READ_COMMAND_BYTE 0x20
#define WRITE_COMMAND_BYTE 0x60
#define DISABLE_MOUSE 0xA7
#define ENABLE_MOUSE 0xA8
#define CHECK_MOUSE_INTERFACE 0xA9
#define WBM 0xD4  // WRITE BYTE TO MOUSE

//PS/2 Mouse
#define LB BIT(0)
#define RB BIT(1)
#define MB BIT(2)
#define BIT3 BIT(3)
#define MSB_X_DELTA BIT(4)
#define MSB_Y_DELTA BIT(5)
#define X_OVFL BIT(6)
#define Y_OVFL BIT(7)

//PS2 MOUSE COMMANDS
#define RESET 0xFF
#define RESEND 0xFE
#define SET_DEF 0xF6
#define DISABLE 0xF5
#define ENABLE 0xF4
#define SET_SAMPLE_RATE 0xF3
#define SET_REMOTE_MODE 0xF0
#define READ_DATA 0xEB
#define SET_STREAM_MODE 0xEA
#define ACK 0xFA
#define NACK 0xFE
#define ERROR 0xFC

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
extern int ps2_hook_id;

void (packet_parser)(struct packet *mouse_packet, uint8_t scan_codes[]);

int (ps2_subscribe_int)(uint8_t *bit_no);

int (ps2_unsubscribe_int)();

void (mouse_ih)();

int (mouse_enable_disable_data)(u32_t command);

uint8_t readMouseByte();

int issueMouseCommand(uint8_t cmd);

int defaultCommand();

int mouseIrqSet(bool enable) ;


