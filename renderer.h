#ifndef RENDERER_H
#define RENDERER_H

#include <inttypes.h>
#include "vertex.h"
#include "color.h"
#include "quad.h"

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
#define TO_SCREEN_Y(y) ((uint16_t)((SCREEN_HEIGHT-(y*SCREEN_HEIGHT))/2.0))
#define TO_SCREEN_X(x) ((uint16_t)((SCREEN_WIDTH+(x*SCREEN_HEIGHT))/2.0))

typedef struct point {
    float x;
    float y;
} point;

typedef struct uv_point {
    unsigned char u;
    unsigned char v;
} uv_point;

#include "texture.h"
#include "triangle.h"
#include "object.h"
#include "screentriangle.h"

void S_set_fov_angle(int angle);
int S_do_gpu_startup();
void S_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t r, uint8_t g, uint8_t b);
void S_draw_tri(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t r, uint8_t g, uint8_t b);
Texture S_load_texture(uint32_t* src, uint16_t x, uint16_t y, uint8_t w, uint8_t h);
void S_upload_image_data(uint32_t* src, uint16_t x, uint16_t y, uint16_t h, uint16_t w);
void S_clear_framebuffer(uint16_t val);
void S_fill_background(uint8_t r, uint8_t g, uint8_t b);
void S_draw_tri_textured(
    uint16_t x0, uint16_t y0,
    uint16_t x1, uint16_t y1,
    uint16_t x2, uint16_t y2,
    Texture texture,
    color24 c,
    uint8_t v0, uint8_t u0,
    uint8_t v1, uint8_t u1,
    uint8_t v2, uint8_t u2
);
void S_draw_quad_textured(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, color24 c);
void project(Vertex* v, ScreenPoint* p);
void draw_triangle(Triangle* triangle);
void draw_quad(Quad* quad);
void clip_and_render(Triangle* triangle);
void Triangle_render(Triangle* triangle);
void Object_render(Object *object);
void S_commit_scene();

#endif //RENDERER_H
