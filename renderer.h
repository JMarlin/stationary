#ifndef RENDERER_H
#define RENDERER_H

#include "vertex.h"
#include "color.h"
#include "triangle.h"
#include "quad.h"
#include "object.h"
#include "screentriangle.h"

#define GPU_DATA_CMD(c, o) ((((c)&0x7)<<5) | ((o)&0x1F))
#define PRIM_POLY 0x01
#define PRIM_LINE 0x02
#define PRIM_SPRITE 0x04
#define OPT_TGE 0x01
#define OPT_ABE 0x02
#define OPT_TME 0x04
#define OPT_VTX 0x08
#define OPT_IIP 0x10

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_PIXELS SCREEN_WIDTH * SCREEN_HEIGHT
#define SCREEN_DEPTH 100.0

//Convert a point scaled such that 1.0, 1.0 is at the upper right-hand
//corner of the screen and -1.0, -1.0 is at the bottom right to pixel coords
#define TO_SCREEN_Y(y) ((int)((SCREEN_HEIGHT-(y*SCREEN_HEIGHT))/2.0))
#define TO_SCREEN_X(x) ((int)((SCREEN_WIDTH+(x*SCREEN_HEIGHT))/2.0))

typedef struct point {
    float x;
    float y;
} point;

void S_set_fov_angle(int angle);
int S_do_gpu_startup();
void S_draw_tri(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned char r, unsigned char g, unsigned char b);
void S_upload_image_data(unsigned long* src, unsigned short x, unsigned short y, unsigned short h, unsigned short w);
void S_clear_framebuffer(unsigned short val);
void S_draw_tri_textured(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, color24 c);
void S_draw_quad_textured(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short x3, unsigned short y3, color24 c);
void project(Vertex* v, ScreenPoint* p);
void draw_triangle(Triangle* triangle);
void draw_quad(Quad* quad);
void clip_and_render(Triangle* triangle);
void Triangle_render(Triangle* triangle);
void Object_render(Object *object);
void S_commit_scene();

#endif //RENDERER_H