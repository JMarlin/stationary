
#include "object.h"
#include "triangle.h"
#include "list.h"
#include "mathhelpers.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

void Object_delete(Object *object) {
    	    
    List_delete(object->triangles, Triangle_deleter);
    free((void*)object);
}

int Object_add_triangle(Object* object, Vertex *v1, Vertex *v2, Vertex *v3) {
	
	Triangle* triangle;
	
	if(!(triangle = Triangle_new(v1, v2, v3)))
	    return 0;
    
	if(!List_add(object->triangles, (void*)triangle)) {
		
		Triangle_delete(triangle);
		return 0;
	}
}

Object *Object_new() {
    
    Object *object = ((Object*)malloc(sizeof(Object)));
    
    if(!object)
        return object;
	    
    if(!(object->triangles = List_new())) {
		
		free((void*)object);
		return (Object*)0;
	}
	
    object->x = object->y = object->z = 0.0;
    
    return object;
}

void Object_translate(Object* object, float x, float y, float z) {
    
    Triangle *triangle;
    int      j;
    
	printf("Updating object origin\n");
	
    object->x += x;
    object->y += y;
    object->z += z;
    
    List_for_each(object->triangles, triangle, Triangle*) {
                
        for(j = 0; j < 3; j++) {
        	
			printf("x(%f) + x(%f) = ", triangle->v[j].x, x);	    
            triangle->v[j].x += x; printf("x(%f)\n", triangle->v[j].x);
			printf("y(%f) + y(%f) = ", triangle->v[j].y, y);	    
            triangle->v[j].y += y; printf("y(%f)\n", triangle->v[j].y);
			printf("z(%f) + z(%f) = ", triangle->v[j].z, z);	    
            triangle->v[j].z += z; printf("z(%f)\n", triangle->v[j].z);
        }
    }
}

void Object_rot_x_global(Object* object, float angle) {
    
    float rad_angle = DEG_TO_RAD(angle);
    Triangle *triangle;
    int j;
    float temp_y, temp_z;
        
    List_for_each(object->triangles, triangle, Triangle*) {
        
        for(j = 0; j < 3; j++) {
        
            temp_y = triangle->v[j].y;
            temp_z = triangle->v[j].z;
        
            triangle->v[j].y = (temp_y * cos(rad_angle)) - (temp_z * sin(rad_angle));
            triangle->v[j].z = (temp_y * sin(rad_angle)) + (temp_z * cos(rad_angle));
        }
    }
}

void Object_rot_y_global(Object* object, float angle) {
    
    float rad_angle = DEG_TO_RAD(angle);
    Triangle *triangle;
    int j;
    float temp_x, temp_z;
        
    List_for_each(object->triangles, triangle, Triangle*) {
                
        for(j = 0; j < 3; j++) {
            
            temp_x = triangle->v[j].x;
            temp_z = triangle->v[j].z;
            
            triangle->v[j].x = (temp_x * cos(rad_angle)) + (temp_z * sin(rad_angle));
            triangle->v[j].z = (temp_z * cos(rad_angle)) - (temp_x * sin(rad_angle));
        }
    }
}

void Object_rot_z_global(Object* object, float angle) {
    
    float rad_angle = DEG_TO_RAD(angle);
    Triangle *triangle;
    int j;
    float temp_x, temp_y;
        
    List_for_each(object->triangles, triangle, Triangle*) {
         
        for(j = 0; j < 3; j++) {
            
            temp_x = triangle->v[j].x;
            temp_y = triangle->v[j].y;
            
            triangle->v[j].x = (temp_x * cos(rad_angle)) - (temp_y * sin(rad_angle));
            triangle->v[j].y = (temp_x * sin(rad_angle)) + (temp_y * cos(rad_angle));
        }
    }
}

void Object_rot_x_local(Object* object, float angle) {
    
    float oldx, oldy, oldz;
    
    oldx = object->x;
    oldy = object->y;
    oldz = object->z;
    
    Object_translate(object, -oldx, -oldy, -oldz);
    Object_rot_x_global(object, angle);
    Object_translate(object, oldx, oldy, oldz);
}

void Object_rot_y_local(Object* object, float angle) {
    
    float oldx, oldy, oldz;
    
    oldx = object->x;
    oldy = object->y;
    oldz = object->z;
    
    Object_translate(object, -oldx, -oldy, -oldz);
    Object_rot_y_global(object, angle);
    Object_translate(object, oldx, oldy, oldz);
}

void Object_rot_z_local(Object* object, float angle) {
    
    float oldx, oldy, oldz;
    
    oldx = object->x;
    oldy = object->y;
    oldz = object->z;
    
    Object_translate(object, -oldx, -oldy, -oldz);
    Object_rot_z_global(object, angle);
    Object_translate(object, oldx, oldy, oldz);
}
