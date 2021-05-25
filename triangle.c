#include "triangle.h"
#include "vertex.h"
#include "color.h"
#include "renderer.h"
#include <stdlib.h>

Triangle *Triangle_new(Vertex *v1, Vertex *v2, Vertex *v3, color24 c, Texture tex) {
    
    Triangle *triangle = ((Triangle*)malloc(sizeof(Triangle)));
    
    if(!triangle)
        return triangle;
        
    Vertex_clone(v1, &(triangle->v[0]));
    Vertex_clone(v2, &(triangle->v[1]));
    Vertex_clone(v3, &(triangle->v[2]));    
	triangle->c = c;
    triangle->tex = tex;

    return triangle;
}

void Triangle_deleter(void* triangle) {
	
	Triangle_delete((Triangle*)triangle);
}

void Triangle_delete(Triangle *triangle) {
	
	free((void*)triangle);
}

