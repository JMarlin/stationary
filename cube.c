#include "object.h"
#include "color.h"
#include "vertex.h"
#include "cube.h"
#include "renderer.h"
#include <stdio.h>

Object* Cube_new(float s, color24 c, Texture tex) {
    
    Object* object = Object_new();
    int i;
	Vertex temp_v[3];
    float half_s = s/2.0;

    float points[][3] = {
        {-half_s,  half_s, -half_s}, //0
        { half_s,  half_s, -half_s}, //1
        { half_s, -half_s, -half_s}, //2
        {-half_s, -half_s, -half_s}, //3
        {-half_s,  half_s,  half_s}, //4
        { half_s,  half_s,  half_s}, //5
        { half_s, -half_s,  half_s}, //6
        {-half_s, -half_s,  half_s}  //7
    };

    const int point_order[][3] = {
                  {7, 5, 4}, //--
                  {6, 5, 7}, //--
                  {3, 0, 1}, //--
                  {3, 1, 2}, //--
                  {4, 5, 0}, //--
                  {1, 0, 5}, //--
                  {6, 7, 3}, //--
                  {3, 2, 6}, //--
                  {6, 1, 5}, //--
                  {6, 2, 1}, //--
                  {7, 4, 0}, //--
                  {0, 3, 7}  //--
              };

	const color24 tag_color[12] = {
	    RGB24(0, 0, 0),    RGB24(0, 0, 0xFF),    RGB24(0, 0xFF, 0),    RGB24(0, 0xFF, 0xFF),
		RGB24(0xFF, 0, 0), RGB24(0xFF, 0, 0xFF), RGB24(0xFF, 0xFF, 0), RGB24(0xFF, 0xFF, 0xFF),
		RGB24(0, 0, 0x88), RGB24(0, 0x88, 0),    RGB24(0, 0x88, 0x88), RGB24(0x88, 0, 0) 	
	};
    
    const uv_point uv_points[] = {
        {.u = 0,     .v = 0    }, //0
        {.u = tex.w, .v = 0    }, //1
        {.u = 0,     .v = tex.h}, //2
        {.u = tex.w, .v = tex.h}  //3
    };

    const int uv_point_order[][3] = {
        { 2, 1, 0 },
        { 3, 1, 2 },
        { 3, 1, 0 },
        { 3, 0, 2 },
        { 2, 3, 0 },
        { 1, 0, 3 },
        { 1, 0, 2 },
        { 2, 3, 1 }, 
        { 2, 1, 0 },
        { 2, 3, 1 },
        { 3, 1, 0 },
        { 0, 2, 3 }
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
     
        temp_v[0].x = points[point_order[i][0]][0];
        temp_v[0].y = points[point_order[i][0]][1];
        temp_v[0].z = points[point_order[i][0]][2];
        temp_v[0].u = uv_points[uv_point_order[i][0]].u;
        temp_v[0].v = uv_points[uv_point_order[i][0]].v;

        temp_v[1].x = points[point_order[i][1]][0];
        temp_v[1].y = points[point_order[i][1]][1];
        temp_v[1].z = points[point_order[i][1]][2];
        temp_v[1].u = uv_points[uv_point_order[i][1]].u;
        temp_v[1].v = uv_points[uv_point_order[i][1]].v;

        temp_v[2].x = points[point_order[i][2]][0];
        temp_v[2].y = points[point_order[i][2]][1];
        temp_v[2].z = points[point_order[i][2]][2];
        temp_v[2].u = uv_points[uv_point_order[i][2]].u;
        temp_v[2].v = uv_points[uv_point_order[i][2]].v;
        
        printf("[new_cube] Creating new triangle (%f, %f, %f), (%f, %f, %f), (%f, %f, %f)\n", temp_v[0].x, temp_v[0].y, temp_v[0].z, temp_v[1].x, temp_v[1].y, temp_v[1].z,  temp_v[2].x, temp_v[2].y, temp_v[2].z);
        
        if(!(Object_add_triangle(
            object,
            &temp_v[0], &temp_v[1], &temp_v[2],
            RGB24(0xFF, 0xFF, 0xFF), tex
        ))) {
            
            printf("[new_cube] failed add new triangle #%d to the object\n", i+1);
            Object_delete(object);
            return (Object*)0;        
        }
		
        printf("[new_cube] inserted triangle #%d\n", i+1);
    }
    
    return object;
}