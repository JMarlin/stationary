#include "zlist.h"
#include "screentriangle.h"
#include "renderer.h"

List* zlist = (List*)0;

int ZList_init() {
	
	return !!(zlist = List_new());
}

void ZList_clear() {
	
	List_delete(zlist, ScreenTriangle_deleter);
}

int ZList_add(ScreenPoint *p1, ScreenPoint *p2, ScreenPoint *p3, color24 c) {
	
	ScreenTriangle *new_screen_triangle, *current_screen_triangle;
	int i;
	
	if(!(new_screen_triangle = ScreenTriangle_new(p1, p2, p3, c)))
	    return 0;
		
	i = 0;
	List_for_each(zlist, current_screen_triangle, ScreenTriangle*) {
		
		if(current_screen_triangle->z > new_screen_triangle->z)
		    break;
			
		i++;
	}
	
	if(!List_add_at(zlist, i, (void*)new_screen_triangle))
	    return 0;
}

void ZList_render() {
	
	ScreenTriangle *screen_triangle;
	
	List_for_each(zlist, screen_triangle, ScreenTriangle*)
	    S_draw_tri_textured(
			screen_triangle->points[0].x,
			screen_triangle->points[0].y,
			screen_triangle->points[1].x,
			screen_triangle->points[1].y, 
			screen_triangle->points[2].x,
			screen_triangle->points[2].x, 
			screen_triangle->c
		);
}
