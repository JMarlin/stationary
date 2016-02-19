#ifndef SCREENTRIANGLE_H
#define SCREENTRIANGLE_H

#include "color.h"

typedef struct ScreenPoint {
    int x;
    int y;
    float z;
} ScreenPoint;

typedef struct ScreenTriangle {
	ScreenPoint points[3];
	float z;
	color24 c;
} ScreenTriangle;

ScreenTriangle* ScreenTriangle_new(ScreenPoint *p1, ScreenPoint *p2, ScreenPoint *p3, color24 c)

#endif //SCREENTRIANGLE_H
