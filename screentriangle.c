#include "screentriangle.h"
#include "color.h"

void ScreenTriangle_deleter(void* screen_triangle) {
	
	ScreenTriangle_delete((ScreenTriangle*)screen_triangle);
}

void ScreenTriangle_delete(ScreenTriangle* screen_triangle) {
	
	free((void*)screen_triangle);
}

ScreenTriangle* ScreenTriangle_new(ScreenPoint *p1, ScreenPoint *p2, ScreenPoint *p3, color24 c) {
	
	float max_z;
	ScreenTriangle *screen_triangle = (ScreenTriangle*)malloc(sizeof(ScreenTriangle));
	
	if(!ScreenTriangle)
	    return ScreenTriangle;
	
	//Unrolled 'find max' loop 	
	max_z = p1->z;
	
	if(p2->z > max_z)
	    max_z = p2->z;
		
    if(p3->z > max_z)
	    max_z = p3->z;
		
	screen_triangle->points[0].x = p1->x;
	screen_triangle->points[0].y = p1->y;
	screen_triangle->points[0].z = p1->z;
	screen_triangle->points[1].x = p2->x;
	screen_triangle->points[1].y = p2->y;
	screen_triangle->points[1].z = p2->z;
	screen_triangle->points[2].x = p3->x;
	screen_triangle->points[2].y = p3->y;
	screen_triangle->points[2].z = p3->z;
	
	screen_triangle->z = max_z;
	screen_triangle->c = c;	
	
	return screen_triangle;
}