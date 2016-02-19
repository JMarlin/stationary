#include "screentriangle.h"
#include "color.h"
#include <stdlib.h>

void ScreenTriangle_deleter(void* screen_triangle) {
	
	ScreenTriangle_delete((ScreenTriangle*)screen_triangle);
}

void ScreenTriangle_delete(ScreenTriangle* screen_triangle) {
	
	free((void*)screen_triangle);
}

ScreenTriangle* ScreenTriangle_new(ScreenPoint *p1, ScreenPoint *p2, ScreenPoint *p3, color24 c) {
	
	float avg_z;
	ScreenTriangle *screen_triangle = (ScreenTriangle*)malloc(sizeof(ScreenTriangle));
	
	if(!screen_triangle)
	    return screen_triangle;
	
	//Unrolled 'find max' loop 	
	avg_z = (p1->z + p2->z + p3->z)/3;
		 
	screen_triangle->points[0].x = p1->x;
	screen_triangle->points[0].y = p1->y;
	screen_triangle->points[0].z = p1->z;
	
	screen_triangle->points[1].x = p2->x;
	screen_triangle->points[1].y = p2->y;
	screen_triangle->points[1].z = p2->z;
	
	screen_triangle->points[2].x = p3->x;
	screen_triangle->points[2].y = p3->y;
	screen_triangle->points[2].z = p3->z;
	
	screen_triangle->z = avg_z;
	screen_triangle->c = c;	
	
	return screen_triangle;
}