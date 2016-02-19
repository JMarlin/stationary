#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vertex.h"
#include "color.h"

typedef struct Triangle {
    Vertex v[3];
	color24 c;
} Triangle;

Triangle *Triangle_new(Vertex *v1, Vertex *v2, Vertex *v3, color24 c);
void Triangle_deleter(void* triangle);
void Triangle_delete(Triangle *triangle);

#endif //TRIANGLE_H