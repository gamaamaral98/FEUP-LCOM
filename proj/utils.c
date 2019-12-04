#include <lcom/lcf.h>
#include "utils.h"
#include <stdint.h>

uint32_t count = 0;

int (util_sys_inb)(int port, uint8_t *value) {

  uint32_t val;

  if(sys_inb(port, &val) != OK) return 1;
  
  *value = (uint8_t)val;

  count++;
 
  return count;

}

int(util_get_LSB)(uint16_t val, uint8_t *lsb) {
  /* To be implemented by the students */
  *lsb = (uint8_t)val;

  return 0;
  
}

int(util_get_MSB)(uint16_t val, uint8_t *msb) {
  /* To be implemented by the students */
  val = val >> 8;

  *msb = (uint8_t)val;

  return 0;

}
