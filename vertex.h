#ifndef VERTEX_H
#define VERTEX_H

#include "color.h"

typedef struct Vertex {
    float x;
    float y;
    float z;
    color24 c;
} Vertex;

void Vertex_clone(Vertex *src, Vertex* dst);

#endif //VERTEX_H
