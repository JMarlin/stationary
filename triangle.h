#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "vertex.h"
#include "color.h"
#include "texture.h"

typedef struct Triangle {
    Vertex v[3];
    color24 c;
    Texture tex; //NOTE: Maybe these should refer to an index instead of the entire texture entry?
} Triangle;

typedef struct TriangleList {
    int count;
    Triangle* triangles;
    int img_w;
    int img_h;
    uint16_t* pixdata;
} TriangleList;

#include "renderer.h"

Triangle *Triangle_new(Vertex *v1, Vertex *v2, Vertex *v3, color24 c, Texture tex);
void Triangle_deleter(void* triangle);
void Triangle_delete(Triangle *triangle);

#endif //TRIANGLE_H
