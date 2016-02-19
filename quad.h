#ifndef QUAD_H
#define QUAD_H

#include "vertex.h"
#include "color.h"

typedef struct Quad {
    Vertex v[4];
	color24 c;
} Quad;

Quad *Quad_new(Vertex *v1, Vertex *v2, Vertex *v3, Vertex *v4, color24 c);
void Quad_deleter(void* quad);
void Quad_delete(Quad *quad);

#endif //QUAD_H