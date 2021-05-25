#ifndef SCREENTRIANGLE_H
#define SCREENTRIANGLE_H

#include "color.h"
#include "texture.h"

typedef struct ScreenPoint {
    unsigned short x;
    unsigned short y;
    float z;
    unsigned char u;
    unsigned char v;
} ScreenPoint;

typedef struct ScreenTriangle {
	ScreenPoint points[3];
	float z;
	color24 c;
    Texture tex;
} ScreenTriangle;

#include "renderer.h"

void ScreenTriangle_deleter(void* screen_triangle);
void ScreenTriangle_delete(ScreenTriangle* screen_triangle);
ScreenTriangle* ScreenTriangle_new(ScreenPoint *p1, ScreenPoint *p2, ScreenPoint *p3, color24 c, Texture tex);

#endif //SCREENTRIANGLE_H
