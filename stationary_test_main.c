#include <stdio.h>
#include "renderer.h"
#include "cube.h"
#include "object.h"
#include "color.h"
#include "screentriangle.h"
#include "peops/externals.h"
#include "peops/gpu.h"

int main(int argc, char* argv[]) {

    unsigned long disp;
    int direction;
    int player_angle = 90, chg_angle = 0;
    float i = 0.0, step = 0, rstep = 0, fps, walkspeed = 0.04;
    Object *cube1, *cube2;
    int done = 0;
    int numFrames = 0; 

    GPUinit();
	GPUopen(&disp, "Stationary", "./peops.cfg");
	//GPUdisplayText("You are now running Stationary");
	//updateDisplay();
	ulKeybits &= ~KEY_SHOWFPS;
	printf("You are now running stationary on display %lu\n", disp);
    printf("The GPU has the following status: 0x%08lx\n", (unsigned long)GPUreadStatus());
    if(!S_do_gpu_startup()) {
		
		printf("Couldn't init GPU\n");
		return 0;
	}
    printf("The GPU has the following status: 0x%08lx\n", (unsigned long)GPUreadStatus());
    
	
	    
    if(!(cube1 = Cube_new(10.0, RGB24(255, 255, 255)))) {
        
        printf("Could not allocate a new cube\n");
        return -1;
    }
    
    if(!(cube2 = Cube_new(1.0, RGB24(255, 255, 255)))) {
        
        printf("Could not allocate a new cube\n");
        return -1;
    }

    printf("Cube created successfully\n");

    S_set_fov_angle(50);

    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    printf("Translating objects...");
    //Object_translate(cube1, 0.0, -6.0, 2.0);
    Object_translate(cube2, 0.0, 0.0, 2.0);
	printf("done\n");
	
    //Object_rot_y_local(cube, 45);
    //Object_rot_x_local(cube, 45);
    //Object_rot_z_local(cube, 45);
    
    printf("Starting render loop\n");
    while(!done) {

        i += step;
        //Object_translate(cube1, 0.0, 0.0, step);
        Object_rot_y_local(cube1, 1);
        Object_rot_x_local(cube1, 1);
        Object_rot_z_local(cube1, 1);        
        Object_rot_x_local(cube2, 1);
        Object_rot_z_local(cube2, 1);

        S_clear_framebuffer(RGB15(0, 10, 31));
        
        //Object_render(cube1);
        Object_render(cube2);  
        
        S_commit_scene();
    }

    return 0;
}
