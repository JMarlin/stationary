#include "quad.h"
#include "vertex.h"
#include "color.h"
#include <stdlib.h>

Quad *Quad_new(Vertex *v1, Vertex *v2, Vertex *v3, Vertex *v4, color24 c) {
    
    Quad *quad = ((Quad*)malloc(sizeof(Quad)));
    
    if(!quad)
        return quad;
        
    Vertex_clone(v1, &(quad->v[0]));
    Vertex_clone(v2, &(quad->v[1]));
    Vertex_clone(v3, &(quad->v[2]));  
	Vertex_clone(v4, &(quad->v[3]));     
	quad->c = c;
    
    return quad;
}

void Quad_deleter(void* quad) {
	
	Quad_delete((Quad*)quad);
}

void Quad_delete(Quad *quad) {
	
	free((void*)quad);
}

