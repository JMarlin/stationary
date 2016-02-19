#include "object.h"
#include "color.h"
#include "vertex.h"
#include "cube.h"
#include <stdio.h>

Object* Cube_new(float s, color24 c) {
    
    Object* object = Object_new();
    int i;
	Vertex temp_v[3];
    float half_s = s/2.0;
    float points[][3] = {
        {-half_s, half_s, -half_s},
        {half_s, half_s, -half_s},
        {half_s, -half_s, -half_s},
        {-half_s, -half_s, -half_s},
        {-half_s, half_s, half_s},
        {half_s, half_s, half_s},
        {half_s, -half_s, half_s},
        {-half_s, -half_s, half_s},
    };
    const int order[][3] = {
                  {7, 5, 4}, 
                  {6, 5, 7}, 
                  {3, 0, 1}, 
                  {3, 1, 2}, 
                  {4, 5, 0}, 
                  {1, 0, 5},
                  {6, 7, 3},
                  {3, 2, 6},
                  {6, 1, 5},
                  {6, 2, 1},
                  {7, 4, 0},
                  {0, 3, 7}
              };
    
    if(!object) {
        
        printf("[new_cube] object allocation failed\n");
        return object;
    }
    
    printf("[new_cube] new object allocated\n");
    
    temp_v[0].c = c;
    temp_v[1].c = c;
    temp_v[2].c = c;
    
    for(i = 0; i < 12; i++) {
     
        temp_v[0].x = points[order[i][0]][0];
        temp_v[0].y = points[order[i][0]][1];
        temp_v[0].z = points[order[i][0]][2];
        temp_v[1].x = points[order[i][1]][0];
        temp_v[1].y = points[order[i][1]][1];
        temp_v[1].z = points[order[i][1]][2];
        temp_v[2].x = points[order[i][2]][0];
        temp_v[2].y = points[order[i][2]][1];
        temp_v[2].z = points[order[i][2]][2];
        
        printf("[new_cube] Creating new triangle (%f, %f, %f), (%f, %f, %f), (%f, %f, %f)\n", temp_v[0].x, temp_v[0].y, temp_v[0].z, temp_v[1].x, temp_v[1].y, temp_v[1].z,  temp_v[2].x, temp_v[2].y, temp_v[2].z);
        
        if(!(Object_add_triangle(object, &temp_v[0], &temp_v[1], &temp_v[2]))) {
            
            printf("[new_cube] failed add new triangle #%d to the object\n", i+1);
            Object_delete(object);
            return (Object*)0;        
        }
		
        printf("[new_cube] inserted triangle #%d\n", i+1);
    }
    
    return object;
}