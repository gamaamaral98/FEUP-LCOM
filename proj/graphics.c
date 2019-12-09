#include "graphics.h"

static unsigned h_res;	        /* Horizontal resolution in pixels */
static unsigned v_res;	        /* Vertical resolution in pixels */
static unsigned bytes_per_pixel; /* Number of VRAM byts per pixel */
static unsigned bits_per_pixel; /* Number of VRAM bits per pixel */

//Colors
static unsigned RedMaskSize;         /* size of direct color red mask in bits */
static unsigned RedFieldPosition;    /* bit position of lsb of red mask */
static unsigned GreenMaskSize;       /* size of direct color green mask in bits */
static unsigned GreenFieldPosition;  /* bit position of lsb of green mask */
static unsigned BlueMaskSize;        /* size of direct color blue mask in bits */
static unsigned BlueFieldPosition;   /* bit position of lsb of blue mask */
static unsigned MemoryModel;        /**< @brief memory model type */

static void *video_mem;         /* frame-buffer VM address (static global variable*/ 

void clear_screen(){
 
    //vg_draw_rectangle(0,0,h_res,v_res,0);

    uint8_t* video_mem_aux = video_mem;
    unsigned pixel = 0;
    unsigned int number_of_pixels = h_res*v_res;

    while (pixel < number_of_pixels){
        uint8_t *index = video_mem_aux + pixel;
        *(index) = 0;
        pixel++;
    }
}

int (set_graph)(uint16_t mode){

    struct minix_mem_range mr;
    int r;

    mr.mr_base = 0;
    mr.mr_limit = OFFSET_1MB;

    if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);
        return -1;
    }

    if(vbe_linear_mode(mode) != 0) return -1;

    return 0;

}

int draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y){

    xpm_image_t img;
   
    //uint8_t *video = video_mem;
    uint8_t *sprite = xpm_load(xpm, XPM_INDEXED, &img);

    unsigned int i = 0;

    while(i < img.height){

        if((i + y) > v_res) return 1;

        unsigned int j = 0;

        while(j < img.width){

            if((j + x) > h_res) return 1; 

            unsigned int index = i * img.width + j;
            fill_pixel(x+j, y+i, sprite[index]);

            j++;
        }

        i++;

    }

    return 0;

}

uint16_t handle_height(uint16_t x, uint16_t y, uint16_t height){
    if(y <= v_res && x <= h_res){
        if(y + height > v_res) return v_res - y;
    }
    return height;
}

uint16_t handle_width(uint16_t x, uint16_t y, uint16_t width){
    if(y <= v_res && x <= h_res){
        if(x + width > h_res) return h_res - x;
    }
    return width;
}

uint32_t handle_color(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step, unsigned int i, unsigned int j){
    if(MemoryModel == PACKED_PIXEL){
        
        uint32_t ret = (first + (j * no_rectangles + i) * step) % (1 << bits_per_pixel);
        return ret;
    
    } else {
        uint32_t red, green, blue, RC, GC, BC;

        RC = (first >> RedFieldPosition); 
        GC = (first >> GreenFieldPosition);
        BC = (first >> BlueFieldPosition);

        red = (RC + i * step) % (1 << RedMaskSize);
        green = (GC + j * step) % (1 << GreenMaskSize);
        blue = (BC + (i + j) * step) % (1 << BlueMaskSize);

        return (red << RedFieldPosition) | (green << GreenFieldPosition) | (blue << BlueFieldPosition);
    }

    return 1;
}

int(vg_draw_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step){
    unsigned int i = 0;
    uint32_t color = first;
    uint16_t width = h_res/no_rectangles, height = v_res/no_rectangles;

    while(i < no_rectangles){

        unsigned int j = 0;

        uint16_t y_pos = height*i;

        while(j < no_rectangles){

            color = handle_color(mode, no_rectangles, first, step, j, i);
            
            uint16_t x_pos = width*j;

            if(vg_draw_rectangle(x_pos, y_pos, width, height, color) != 0) return 1;

            j++;
        }
        i++;
    }

    return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
    //Aqui é suposto chamar a função draw_line num ciclo para a height do rectangulo
    unsigned int i;
    for(i = 0; i < height; i++){

        int newY = y + i;
        if(vg_draw_hline(x,newY,width,color) != 0) return 1;

    }
    return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color){
    //preencher cada pixel da linha a desenhar
    unsigned int i;
    for(i = 0; i < len; i++){

        if(fill_pixel(x+i, y, color) != 0) return 1;

    }
    return 0;
}

int (fill_pixel)(uint16_t x, uint16_t y, uint32_t color){
    uint8_t *video_mem_aux = video_mem;

    //Ir para a posição do pixel
    video_mem_aux = video_mem_aux + (h_res * bytes_per_pixel * y);
    video_mem_aux = video_mem_aux + (bytes_per_pixel * x);

    memcpy(video_mem_aux, &color, bytes_per_pixel);

    return 0;
}

int (vbe_linear_mode)(uint16_t mode){
    reg86_t r;

    memset(&r, 0, sizeof(r));

    r.intno = VBE_INT_NO;
    r.ax = VBE_SET_VIDEO_MODE_CMD;
    r.bx = (1<<14)|mode;
    
    if(sys_int86(&r) != OK){
        printf("set_vbe_mode: sys_int86() failed \n");
        return 1;
    }

    return 0;

}

void *(vg_init)(uint16_t mode){

        struct minix_mem_range mr;
        int r;
        
        vbe_mode_info_t vbe_info;

        if(my_vbe_get_mode_info(mode, &vbe_info) != 0){
            printf("Error on vbe_get_info()\n");
        }

        h_res = vbe_info.XResolution;
        v_res = vbe_info.YResolution;
        bits_per_pixel = vbe_info.BitsPerPixel;
        bytes_per_pixel = vbe_info.BitsPerPixel;

        if((bytes_per_pixel % 2) == 0) bytes_per_pixel = bytes_per_pixel/8;
        else{
            bytes_per_pixel++;
            bytes_per_pixel = bytes_per_pixel/8;
        }

        RedFieldPosition = vbe_info.RedFieldPosition;
        RedMaskSize = vbe_info.RedMaskSize;
        GreenFieldPosition = vbe_info.GreenFieldPosition;
        GreenMaskSize = vbe_info.GreenMaskSize;
        BlueFieldPosition = vbe_info.BlueFieldPosition;
        BlueMaskSize = vbe_info.BlueMaskSize;

        MemoryModel = vbe_info.MemoryModel;

        unsigned int vram_base = vbe_info.PhysBasePtr;
        unsigned int vram_size = h_res*v_res*bytes_per_pixel;

        mr.mr_base = (phys_bytes) vram_base;	
        mr.mr_limit = mr.mr_base + vram_size;

        if( OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))){
            panic("sys_privctl (ADD_MEM) failed: %d\n", r);
            return NULL;
        }

        video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

        if(video_mem == MAP_FAILED){
            panic("couldn't map video memory");
            return NULL;    
        }

        if(vbe_linear_mode(mode) != 0) return NULL;

        return video_mem;
}

int (my_vbe_get_mode_info)(uint16_t mode, vbe_mode_info_t *vmi_p){

    mmap_t map;

    lm_alloc(sizeof(vbe_mode_info_t), &map);

    reg86_t r;
    phys_bytes phys = map.phys;

    memset(&r, 0, sizeof(r));

    r.intno = VBE_INT_NO;
    r.ax = VBE_MODE;

    r.es = PB2BASE(phys);
    r.di = PB2OFF(phys);
    
    r.cx = mode;    

    if(sys_int86(&r) != OK){
        printf("set_vbe_mode: sys_int86() failed \n");
        return 1;
    }

    *vmi_p = *((vbe_mode_info_t*) map.virt);

    lm_free(&map);

    return 0;

}

