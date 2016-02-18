#include <stdio.h>
#include "peops/externals.h"
#include "peops/gpu.h"

#define GPU_DATA_CMD(c, o) ((((c)&0x7)<<5) | ((o)&0x1F))
#define PRIM_POLY 0x01
#define PRIM_LINE 0x02
#define PRIM_SPRITE 0x04
#define OPT_TGE 0x01
#define OPT_ABE 0x02
#define OPT_TME 0x04
#define OPT_VTX 0x08
#define OPT_IIP 0x10

void S_do_gpu_startup() {
	
	GPUwriteStatus(0x00000000); //Reset GPU
	GPUwriteStatus(0x06CDA1F4); //Set horizontal display range to the max and min values of 0x1F4 - 0xCDA
	GPUwriteStatus(0x07040010); //Set vertical display range to the default NTSC values of 0x010-0x100	 
	GPUwriteStatus(0x08000001); //Set video mode to NTSC 15-bit non-interlaced 320x240
	GPUwriteStatus(0x05000000); //Set x/y start of display area to (0,0)
	GPUwriteData(0xE1000300); //Set draw mode
	GPUwriteData(0xE3000000); //Set framebuffer drawing area top left to (0, 0)
	GPUwriteData(0xE403C140); //Set framebuffer drawing area bottom right to (320, 240)
	GPUwriteData(0xE5000000); //Set framebuffer drawing area offset to
	GPUwriteStatus(0x03000000); //Enable the GPU
}

void S_draw_tri(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned char r, unsigned char g, unsigned char b) {
    
    //Poly, one color, flat shaded
    GPUwriteData(
        (GPU_DATA_CMD(PRIM_POLY, 0) << 24) |
        b << 16 |
        g << 8 |
        r 
    );
    
    //Vertex 1
    GPUwriteData((y0 << 16) | x0);
    
    //Vertex 2
    GPUwriteData((y1 << 16) | x1);
    
    //Vertex 3
    GPUwriteData((y2 << 16) | x2);
}

void S_upload_image_data(unsigned long* src, unsigned short x, unsigned short y, unsigned short h, unsigned short w) {
	
	int pixel_count = (h * w) >> 1;
	int i;
	
	GPUwriteData(0x01000000); //Reset the command buffer
	GPUwriteData(0xA0000000); //Copy image data to GPU command
	GPUwriteData((y << 16) | x); //Send x and y of destination
	GPUwriteData((h << 16) | w); //Send h and w of image
	
	for(i = 0; i < pixel_count; i++)
	    GPUwriteData(src[i]);
}

void S_clear_framebuffer(unsigned short val) {
	
	int pixel_count = (1024 * 512) >> 1;
	int i;
	
	GPUwriteData(0x01000000); //Reset the command buffer
	GPUwriteData(0xA0000000); //Copy image data to GPU command
	GPUwriteData(0x00000000); //Send x and y of destination
	GPUwriteData((512 << 16) | 1024); //Send h and w of image
	
	for(i = 0; i < pixel_count; i++)
	    GPUwriteData((val << 16) | val);
}

void S_draw_tri_textured(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned char r, unsigned char g, unsigned char b) {
    
	//Make sure our texture is in the vram
	static unsigned long texture[] = {
        0xFFFFFFFF, 0x8000FFFF, 0xFFFF8000, 0xFFFFFFFF,
        0xFFFFFFFF, 0x8000FFFF, 0xFFFF8000, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFF8000, 0x8000FFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFF8000, 0x8000FFFF, 0xFFFFFFFF,
        0x8000FFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF8000,
        0x8000FFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF8000,
        0xFFFF8000, 0xFFFFFFFF, 0xFFFFFFFF, 0x8000FFFF,
        0x80008000, 0x80008000, 0x80008000, 0x80008000
    };
	
	S_upload_image_data(&texture[0], 0, 256, 8, 8);
	
    //Poly, one color, flat shaded
    GPUwriteData(
        (GPU_DATA_CMD(PRIM_POLY, OPT_TME) << 24) |
        b << 16 |
        g << 8 |
        r 
    );
    
    //Vertex 1
    GPUwriteData((y0 << 16) | x0);
    
	//Clut ID and texture location 1
	GPUwriteData(0x00000008); //no CLUT, v = 0, u = 8
	
    //Vertex 2
    GPUwriteData((y1 << 16) | x1);
    
	//Texture page info and second texture location
	GPUwriteData(0x01900400); //Use 15-bit direct texture at (0,256) -- v = 4, u = 0 
	
    //Vertex 3
    GPUwriteData((y2 << 16) | x2);
	
	//Third texture location
	GPUwriteData(0x00000808); //v = 8, u = 8
}

int main(int argc, char* argv[]) {
	
	unsigned long disp;
    unsigned long status;
	
	GPUinit();
	GPUopen(&disp, "Stationary", "./peops.cfg");
	//GPUdisplayText("You are now running Stationary");
	//updateDisplay();
	ulKeybits &= ~KEY_SHOWFPS;
	printf("You are now running stationary on display %lu\n", disp);
    printf("The GPU has the following status: 0x%08lx\n", (unsigned long)GPUreadStatus());
    S_do_gpu_startup();
    printf("The GPU has the following status: 0x%08lx\n", (unsigned long)GPUreadStatus());
	S_clear_framebuffer(0xFF00);
	S_draw_tri(0, 0, 320, 240, 0, 240, 0xFF, 0x00, 0x00);
	S_draw_tri(0, 0, 320, 0, 320, 240, 0x00, 0xFF, 0x00);
    S_draw_tri_textured(0, 240, 160, 0, 320, 240, 0xFF, 0xFF, 0xFF);
    updateDisplay();
    while(1); 
	return 1;
}