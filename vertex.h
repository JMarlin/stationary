#ifndef VERTEX_H
#define VERTEX_H

typedef struct Vertex {
    float x;
    float y;
    float z;
    color24 c;
} vertex;

void Vertex_clone(Vertex *src, Vertex* dst);

#endif //VERTEX_H
