#ifndef OBJECT_H
#define OBJECT_H

#include "list.h"
#include "vertex.h"
#include "color.h"

typedef struct Object {
    List* triangles;
    float x;
    float y;
    float z;
} Object;


void Object_delete(Object *object);
int Object_add_triangle(Object* object, Vertex *v1, Vertex *v2, Vertex *v3, color24 c);
Object *Object_new();
void Object_translate(Object* object, float x, float y, float z);
void Object_rot_x_global(Object* object, float angle);
void Object_rot_y_global(Object* object, float angle);
void Object_rot_z_global(Object* object, float angle);
void Object_rot_x_local(Object* object, float angle);
void Object_rot_y_local(Object* object, float angle);
void Object_rot_z_local(Object* object, float angle);


#endif //OBJECT_H
