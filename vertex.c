#include "vertex.h"

void Vertex_clone(Vertex *src, Vertex* dst) {
    
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
    dst->c = src->c;
    dst->u = src->u;
    dst->v = src->v;
}