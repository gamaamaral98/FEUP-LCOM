#include <lcom/lcf.h>
#include "utils.h"

uint32_t count = 0;

int (util_sys_inb)(int port, uint8_t *value) {

  uint32_t val;

  if(sys_inb(port, &val) != OK) return 1;
  
  *value = (uint8_t)val;

  count++;
 
  return count;

}
