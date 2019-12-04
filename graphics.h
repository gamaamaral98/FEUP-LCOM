#include <lcom/lcf.h>

//To use tickdelay
#include <minix/sysutil.h>
#include <minix/driver.h>
#include <sys/mman.h>

#define OFFSET_1MB 1024*1024
#define VBE_INT_NO 0x10
#define VBE_SET_VIDEO_MODE_CMD 0x4F02
#define VBE_MODE 0x4F01
#define PACKED_PIXEL    0x04
#define DIRECT_COLOR    0x06

#define MODE 0x105

void *(vg_init)(uint16_t mode);

int (set_graph)(uint16_t mode);

int (vbe_linear_mode)(uint16_t mode);

int(vg_draw_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step);

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color);

int (fill_pixel)(uint16_t x, uint16_t y, uint32_t color);

uint32_t handle_color(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step, unsigned int i, unsigned int j);

uint16_t handle_height(uint16_t x, uint16_t y, uint16_t height);

uint16_t handle_width(uint16_t x, uint16_t y, uint16_t width);

int draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y);

int (my_vbe_get_mode_info)(uint16_t mode, vbe_mode_info_t *vmi_p);

void clear_screen();
