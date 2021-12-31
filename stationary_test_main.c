#include <stdio.h>
#include "renderer.h"
#include "cube.h"
#include "object.h"
#include "color.h"
#include "screentriangle.h"
#include "peops/externals.h"
#include "peops/gpu.h"
#include "tz_text.h"

#include "test_obj.h"

#ifdef NATIVE_VERSION
#include <SDL2/SDL.h>
#else
#include "webshim.h"
#endif

int ProcessEvents();
void RenderScene();

#warning Consolidate global state into a game object
Object *cube1, *cube2, *mesh1;
Texture tex;
int mouse_x = 0;
int mouse_y = 0;
int buttons = 0;

int main(int argc, char* argv[]) {

    void* disp;
    int direction;
    int player_angle = 90, chg_angle = 0;
    //float i = 0.0, step = 0, rstep = 0, fps, walkspeed = 0.04;    
    int done = 0;
    int numFrames = 0; 

#ifdef NATIVE_VERSION
    SDL_Init(0);
    SDL_InitSubSystem(SDL_INIT_EVENTS);
#else
    WS_InitEvents();
#endif

  GPUinit();
    #warning Make configuration not dependent on reading from FS
	GPUopen(&disp, "Stationary", "./peops.cfg");
	//GPUdisplayText("You are now running Stationary");
	//updateDisplay();
	ulKeybits &= ~KEY_SHOWFPS;
	printf("You are now running stationary on display %lx\n", (unsigned long)disp);
    printf("The GPU has the following status: 0x%08lx\n", (unsigned long)GPUreadStatus());
    if(!S_do_gpu_startup()) {
		
		printf("Couldn't init GPU\n");
		return 0;
	}
    printf("The GPU has the following status: 0x%08lx\n", (unsigned long)GPUreadStatus());
/*    
#define BL 0x8000
#define WH 0xFFFF

    //Make sure our texture is in the vram
	static uint16_t texture[] = {
        BL, BL, BL, BL, BL, BL, BL, BL,
        BL, BL, WH, WH, WH, WH, WH, BL,
        BL, WH, BL, WH, WH, WH, WH, BL,
        BL, WH, WH, BL, WH, WH, WH, BL,
        BL, WH, WH, WH, BL, WH, WH, BL,
        BL, WH, WH, WH, WH, BL, WH, BL,
        BL, WH, WH, WH, WH, WH, BL, BL
        BL, BL, BL, BL, BL, BL, BL, BL,
    };

    tex = S_load_texture(
        (uint32_t*)texture, 0, 256,
        8, 8
    );
*/	    
/*
    tex = S_load_texture(
        (uint32_t*)tz_text, 0, 256, 32, 128
    );
*/
    /*
    if(!(cube1 = Cube_new(1.5, RGB24(255, 255, 255), tex))) {
        
        printf("Could not allocate a new cube\n");
        return -1;
    }
    
    if(!(cube2 = Cube_new(1.0, RGB24(255, 255, 255), tex))) {
        
        printf("Could not allocate a new cube\n");
        return -1;
    }

    printf("Cube created successfully\n";
*/

    if(!(mesh1 = Object_from_triangles(&test_obj_mesh))) {

        printf("Could not build mesh object\n");
        return -1;
    }

    S_set_fov_angle(45);

    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    printf("Translating objects...");
    //Object_translate(cube1, 0.0, 0.0, 2.5);
    //Object_translate(cube2, 0.0, 0.0, 4);
    Object_translate(mesh1, 0.0, -4.0, 7);
    printf("done\n");

    //Object_rot_z_local(cube2, 90);
    //Object_scale_x(cube2, 4.0);
    //Object_rot_x_local(cube2, 125);
    
    printf("Starting render loop\n");
#ifdef NATIVE_VERSION

    SDL_ShowCursor(SDL_DISABLE);

    while(!done) {

        done = ProcessEvents();
        RenderScene();   
    }        
#else
    WS_StartEventDispatch(ProcessEvents);
    WS_SetRenderLoopProc(RenderScene);
#endif 

#ifdef NATIVE_VERSION
    SDL_Quit();
#else
    //In web version, even though main exits
    //our event and render loops will continue
    //executing after this
#endif

    return 0;
}

void ClampMouse() {

	mouse_x = mouse_x < 0 ? 0 : mouse_x > 319 ? 319 : mouse_x;
	mouse_y = mouse_y < 0 ? 0 : mouse_y > 239 ? 239 : mouse_y;
}

int line[2][2] = { { 0, 0 }, { 0, 0 } };
int clickState = 0;

int HandleMouseClick() {

	line[clickState][0] = mouse_x;
	line[clickState][1] = mouse_y;

	clickState++;

	if(clickState == 2)
		clickState == 0;
}

int ProcessEvents() {

#ifdef NATIVE_VERSION
    SDL_Event event;
    int done = 0;

    while(SDL_PollEvent(&event)) {

        if(event.type == SDL_QUIT) {

            printf("Quittin' time...\n");

            done = 1;
        }

        if(event.type == SDL_MOUSEMOTION) {

            SDL_MouseMotionEvent* mouse_evt = (SDL_MouseMotionEvent*)&event;

            mouse_x = mouse_evt->x >> 1;
            mouse_y = mouse_evt->y >> 1;

	    ClampMouse();
        }
    }

    return done;
#else
    #warning Not Implemented: web version of event handling

    int old_buttons = buttons;

    WS_GetMouse(&mouse_x, &mouse_y, &buttons);
    ClampMouse();

    if(old_buttons == 1 && buttons == 0)
	    HandleMouseClick();

    return 0;
#endif
}

#define OP_PLACE_CAM 1
#define OP_MOVE_CAM 2

typedef struct Vec3_S {
    float x;
    float y;
    float z;
} Vec3;

typedef struct Instruction_S {
    int operation;
    int frames;
    int wait;
    Vec3 operand;
} Instruction;

Instruction testInstructions[] = {
    {
        OP_PLACE_CAM,
	0,
	0,
	{ 0, 0, -20 }
    },
    {
        OP_MOVE_CAM,
	200,
	0,
	{ 0, 0, 0 }
    },
    {
	OP_MOVE_CAM,
	350,
	0,
	{ 0, 0, -20 }
    },
    {
	OP_MOVE_CAM,
	100,
	0,
	{ 5, 0, 0 }
    },
    {
	OP_MOVE_CAM,
	200,
	0,
	{ -5, 0, 0 }
    },
    {
	OP_MOVE_CAM,
	100,
	0,
	{ 0, 0, 0 }
    },
    {
	OP_MOVE_CAM,
	50,
	0,
	{ 0, 0, -20 }
    },
    {0}
};

void ProcessAnimation(Instruction* instructions) {
	static int position = -1;
	static int frames_remaining = 0;
	static Vec3 delta = { 0.0, 0.0, 0.0 };
	static Vec3 current = { 0.0, 0.0, 0.0 };

	int frame_ready = 0;

	while(!frame_ready) {

		if(!frames_remaining)
		{
			position++;

			if(!instructions[position].operation)
				position = 0;

			if(instructions[position].operation == OP_PLACE_CAM) {
				delta.x = instructions[position].operand.x - current.x;
				delta.y = instructions[position].operand.y - current.y;
				delta.z = instructions[position].operand.z - current.z;

				frames_remaining = 1;
			}
				
			if(instructions[position].operation == OP_MOVE_CAM) {
				delta.x = (instructions[position].operand.x - current.x) / instructions[position].frames;
				delta.y = (instructions[position].operand.y - current.y) / instructions[position].frames;
				delta.z = (instructions[position].operand.z - current.z) / instructions[position].frames;

				frames_remaining = instructions[position].frames;
				frame_ready = 1;
			}

			printf("next delta: {%f, %f, %f}\n", delta.x, delta.y, delta.z);
		} else {
			frame_ready = 1;
		}

		current.x += delta.x;
		current.y += delta.y;
		current.z += delta.z;
		Object_translate(mesh1, -delta.x, -delta.y, -delta.z);
		Object_rot_y_local(mesh1, 0.6);
		Object_rot_x_local(mesh1, 0.4);
		frames_remaining--;
	}
}

void RenderScene() {

    //Object_rot_y_local(mesh1, 0.5);
    //Object_rot_z_local(mesh1, 0.7);        
    //Object_rot_x_local(mesh1, 0.9);

    ProcessAnimation(testInstructions);
    
    S_fill_background(0, 0x30, 0xF0);
    
    Object_render(mesh1);  

    S_commit_scene();

    S_draw_tri(
            mouse_x, mouse_y,
            mouse_x + 12, mouse_y + 4,
            mouse_x + 4, mouse_y + 12,
            128, 128, 128
    );

    S_draw_line(clickState == 0 ? line[0][0] : mouse_x, clickState == 0 ? line[0][1] : mouse_y, clickState == 1 ? mouse_x : line[1][0], clickState == 1 ? mouse_y : line[1][1], 0, 0, 0);

    updateDisplay();
}
