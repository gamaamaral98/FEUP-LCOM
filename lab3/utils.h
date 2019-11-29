#include <lcom/lcf.h>

#include <minix/sysutil.h>

//extern uint32_t count; // contador  numero de vezes call sys inb
int (util_sys_inb)(int port, uint8_t *value);

extern uint32_t count;
