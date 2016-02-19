#include "triangle.h"
#include "vertex.h"
#include <stdlib.h>

Triangle *Triangle_new(Vertex *v1, Vertex *v2, Vertex *v3) {
    
    Triangle *triangle = ((Triangle*)malloc(sizeof(Triangle)));
    
    if(!triangle)
        return triangle;
        
    Vertex_clone(v1, &(triangle->v[0]));
    Vertex_clone(v2, &(triangle->v[1]));
    Vertex_clone(v3, &(triangle->v[2]));    
    
    return triangle;
}

void Triangle_deleter(void* triangle) {
	
	Triangle_delete((Triangle*)triangle);
}

void Triangle_delete(Triangle *triangle) {
	
	free((void*)triangle);
}

