#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include "peops/externals.h"
#include "peops/gpu.h"

#define GPU_DATA_CMD(c, o) ((((c)&0x7)<<5) | ((o)&0x1F))
#define PRIM_POLY 0x01
#define PRIM_LINE 0x02
#define PRIM_SPRITE 0x04
#define OPT_TGE 0x01
#define OPT_ABE 0x02
#define OPT_TME 0x04
#define OPT_VTX 0x08
#define OPT_IIP 0x10

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define SCREEN_PIXELS SCREEN_WIDTH * SCREEN_HEIGHT
#define SCREEN_DEPTH 100.0

//Convert a point scaled such that 1.0, 1.0 is at the upper right-hand
//corner of the screen and -1.0, -1.0 is at the bottom right to pixel coords
#define PI 3.141592653589793
#define TO_SCREEN_Y(y) ((int)((SCREEN_HEIGHT-(y*SCREEN_HEIGHT))/2.0))
#define TO_SCREEN_X(x) ((int)((SCREEN_WIDTH+(x*SCREEN_HEIGHT))/2.0))
#define TO_SCREEN_Z(z) ((unsigned short)((z) > SCREEN_DEPTH || z < 0 ? 65535 : ((z*65535.0)/SCREEN_DEPTH)))
#define DEG_TO_RAD(a) ((((float)a)*PI)/180.0)

float focal_length;
unsigned short *zbuf;

typedef struct point {
    float x;
    float y;
} point;

typedef struct screen_point {
    int x;
    int y;
    unsigned short z;
} screen_point;

typedef struct color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} color;

typedef struct vertex {
    float x;
    float y;
    float z;
    color *c;
} vertex;

typedef struct triangle {
    vertex v[3];
} triangle;

typedef struct node {
    void *payload;
    struct node *next;
} node;

typedef struct list {
    node *root;
} list;

typedef struct object {
    list tri_list;
    float x;
    float y;
    float z;
} object;

#define list_for_each(l, i, n) for((i) = (l)->root, (n) = 0; (i) != NULL; (i) = (i)->next, (n)++)
#define new(x) ((x*)malloc(sizeof(x)))

void S_do_gpu_startup() {
	
	GPUwriteStatus(0x00000000); //Reset GPU
	GPUwriteStatus(0x06CDA1F4); //Set horizontal display range to the max and min values of 0x1F4 - 0xCDA
	GPUwriteStatus(0x07040010); //Set vertical display range to the default NTSC values of 0x010-0x100	 
	GPUwriteStatus(0x08000001); //Set video mode to NTSC 15-bit non-interlaced 320x240
	GPUwriteStatus(0x05000000); //Set x/y start of display area to (0,0)
	GPUwriteData(0xE1000300); //Set draw mode
	GPUwriteData(0xE3000000); //Set framebuffer drawing area top left to (0, 0)
	GPUwriteData(0xE403C140); //Set framebuffer drawing area bottom right to (320, 240)
	GPUwriteData(0xE5000000); //Set framebuffer drawing area offset to
	GPUwriteStatus(0x03000000); //Enable the GPU
}

void S_draw_tri(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned char r, unsigned char g, unsigned char b) {
    
    //Poly, one color, flat shaded
    GPUwriteData(
        (GPU_DATA_CMD(PRIM_POLY, 0) << 24) |
        b << 16 |
        g << 8 |
        r 
    );
    
    //Vertex 1
    GPUwriteData((y0 << 16) | x0);
    
    //Vertex 2
    GPUwriteData((y1 << 16) | x1);
    
    //Vertex 3
    GPUwriteData((y2 << 16) | x2);
}

void S_upload_image_data(unsigned long* src, unsigned short x, unsigned short y, unsigned short h, unsigned short w) {
	
	int pixel_count = (h * w) >> 1;
	int i;
	
	GPUwriteData(0x01000000); //Reset the command buffer
	GPUwriteData(0xA0000000); //Copy image data to GPU command
	GPUwriteData((y << 16) | x); //Send x and y of destination
	GPUwriteData((h << 16) | w); //Send h and w of image
	
	for(i = 0; i < pixel_count; i++)
	    GPUwriteData(src[i]);
}

void S_clear_framebuffer(unsigned short val) {
	
	int pixel_count = (1024 * 512) >> 1;
	int i;
	
	GPUwriteData(0x01000000); //Reset the command buffer
	GPUwriteData(0xA0000000); //Copy image data to GPU command
	GPUwriteData(0x00000000); //Send x and y of destination
	GPUwriteData((512 << 16) | 1024); //Send h and w of image
	
	for(i = 0; i < pixel_count; i++)
	    GPUwriteData((val << 16) | val);
}

void S_draw_tri_textured(unsigned short x0, unsigned short y0, unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned char r, unsigned char g, unsigned char b) {
    
	//Make sure our texture is in the vram
	static unsigned long texture[] = {
        0xFFFFFFFF, 0x8000FFFF, 0xFFFF8000, 0xFFFFFFFF,
        0xFFFFFFFF, 0x8000FFFF, 0xFFFF8000, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFF8000, 0x8000FFFF, 0xFFFFFFFF,
        0xFFFFFFFF, 0xFFFF8000, 0x8000FFFF, 0xFFFFFFFF,
        0x8000FFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF8000,
        0x8000FFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFF8000,
        0xFFFF8000, 0xFFFFFFFF, 0xFFFFFFFF, 0x8000FFFF,
        0x80008000, 0x80008000, 0x80008000, 0x80008000
    };
	
	S_upload_image_data(&texture[0], 0, 256, 8, 8);
	
    //Poly, one color, flat shaded
    GPUwriteData(
        (GPU_DATA_CMD(PRIM_POLY, OPT_TME) << 24) |
        b << 16 |
        g << 8 |
        r 
    );
    
    //Vertex 1
    GPUwriteData((y0 << 16) | x0);
    
	//Clut ID and texture location 1
	GPUwriteData(0x00000800); //no CLUT, v = 8, u = 0
	
    //Vertex 2
    GPUwriteData((y1 << 16) | x1);
    
	//Texture page info and second texture location
	GPUwriteData(0x01900004); //Use 15-bit direct texture at (0,256) -- v = 0, u = 4 
	
    //Vertex 3
    GPUwriteData((y2 << 16) | x2);
	
	//Third texture location
	GPUwriteData(0x00000808); //v = 8, u = 8
}

void clear_zbuf() {
    
    memset((void*)zbuf, 255, SCREEN_PIXELS*2);  
}

int init_zbuf() {
    
    zbuf = (unsigned short*)malloc(SCREEN_PIXELS*2);
    
    if(!zbuf)
        return 0;
    
    clear_zbuf();  
        
    return 1;
}

void clone_color(color* src, color* dst) {
    
    dst->r = src->r;
    dst->g = src->g;
    dst->b = src->b;
    dst->a = src->a;
}

node *list_get_last(list *target) {
    
    int i;
    node *list_node;
    
    list_for_each(target, list_node, i) {
        
        if(list_node->next == NULL)
            return list_node;
    }
    
    return NULL;
}

void list_push(list *target, void* item) {
    
    node *last     = list_get_last(target);
    node *new_node = new(node);

    if(!new_node) 
         return;
    

    new_node->payload = item;
    new_node->next = NULL;
        
    if(!last) {
     
        target->root = new_node;
    } else {      
     
        last->next = new_node;
    }
}

void dump_list(list *target) {
    
    node *item;
    
    printf("list.root = 0x%08x\n", (unsigned int)target->root);
    item = target->root;
    
    while(item) {
        
        printf("   Item 0x%08x:\n", (unsigned int)item);
        printf("      payload: 0x%08x\n", (unsigned int)item->payload);
        printf("      next: 0x%08x\n", (unsigned int)item->next);
        item = item->next;
    }
}

color *new_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    
    color *ret_color = new(color);
    
    if(!ret_color)
        return ret_color;
    
    ret_color->r = r;
    ret_color->g = g;
    ret_color->b = b;
    ret_color->a = a;
    
    return ret_color;
}

void clone_vertex(vertex *src, vertex* dst) {
    
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
    dst->c = src->c;
}

triangle *new_triangle(vertex *v1, vertex *v2, vertex *v3) {
    
    triangle *ret_tri = new(triangle);
    
    if(!ret_tri)
        return ret_tri;
        
    clone_vertex(v1, &(ret_tri->v[0]));
    clone_vertex(v2, &(ret_tri->v[1]));
    clone_vertex(v3, &(ret_tri->v[2]));    
    
    return ret_tri;
}

void purge_list(list *target) {
    
    node *temp_node, *next_node;
    
    for(temp_node = target->root; temp_node != NULL;) {
        next_node = temp_node->next;
        free(temp_node);
        temp_node = next_node;
    }
}

void delete_object(object *obj) {
    
    node *item;
    int i;
    
    list_for_each(&(obj->tri_list), item, i) {
        
        free(item->payload);
    }
    
    purge_list(&(obj->tri_list));
    free(obj);
}

object *new_object() {
    
    object *ret_obj = new(object);
    
    if(!ret_obj)
        return ret_obj;
        
    ret_obj->tri_list.root = NULL;
    ret_obj->x = ret_obj->y = ret_obj->z = 0.0;
    
    return ret_obj;
}

object *new_cube(float s, color *c) {
    
    object* ret_obj = new_object();
    int i;
        triangle *temp_tri;
    vertex temp_v[3];
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
                  {7, 5, 4}, //3
                  {6, 5, 7}, //3
                  {3, 0, 1}, //1
                  {3, 1, 2}, //1
                  {4, 5, 0}, 
                  {1, 0, 5},
                  {6, 7, 3},
                  {3, 2, 6},
                  {6, 1, 5},
                  {6, 2, 1},
                  {7, 4, 0},
                  {0, 3, 7}
              };
    
    if(!ret_obj) {
        
        printf("[new_cube] object allocation failed\n");
        return ret_obj;
    }
    
    printf("[new_cube] new object allocated\n");
        
    if(!ret_obj)
        return ret_obj;
    
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
        
        if(!(temp_tri = new_triangle(&temp_v[0], &temp_v[1], &temp_v[2]))) {
            
            printf("[new_cube] failed to allocate triangle #%d\n", i+1);
            delete_object(ret_obj);
            return NULL;        
        }
        printf("[new_cube] generated triangle #%d\n", i+1);
        
        list_push(&(ret_obj->tri_list), (void*)temp_tri);
        printf("[new_cube] inserted triangle #%d\n", i+1);
    }
    
    return ret_obj;
}

void translate_object(object* obj, float x, float y, float z) {
    
    triangle *temp_tri;
    node     *item;
    int      i, j;
    
    obj->x += x;
    obj->y += y;
    obj->z += z;
    
    list_for_each(&(obj->tri_list), item, i) {
        
        temp_tri = (triangle*)(item->payload);
        
        for(j = 0; j < 3; j++) {
        
            temp_tri->v[j].x += x;
            temp_tri->v[j].y += y;
            temp_tri->v[j].z += z;
        }
    }
}

void rotate_object_x_global(object* obj, float angle) {
    
    float rad_angle = DEG_TO_RAD(angle);
    triangle *temp_tri;
    node     *item;
    int      i, j;
    float temp_y, temp_z;
        
    list_for_each(&(obj->tri_list), item, i) {
        
        temp_tri = (triangle*)(item->payload);
        
        for(j = 0; j < 3; j++) {
        
            temp_y = temp_tri->v[j].y;
            temp_z = temp_tri->v[j].z;
        
            temp_tri->v[j].y = (temp_y * cos(rad_angle)) - (temp_z * sin(rad_angle));
            temp_tri->v[j].z = (temp_y * sin(rad_angle)) + (temp_z * cos(rad_angle));
        }
    }
}

void rotate_object_y_global(object* obj, float angle) {
    
    float rad_angle = DEG_TO_RAD(angle);
    triangle *temp_tri;
    node     *item;
    int      i, j;
    float temp_x, temp_z;
        
    list_for_each(&(obj->tri_list), item, i) {
        
        temp_tri = (triangle*)(item->payload);
        
        for(j = 0; j < 3; j++) {
            
            temp_x = temp_tri->v[j].x;
            temp_z = temp_tri->v[j].z;
            
            temp_tri->v[j].x = (temp_x * cos(rad_angle)) + (temp_z * sin(rad_angle));
            temp_tri->v[j].z = (temp_z * cos(rad_angle)) - (temp_x * sin(rad_angle));
        }
    }
}

void rotate_object_z_global(object* obj, float angle) {
    
    float rad_angle = DEG_TO_RAD(angle);
    triangle *temp_tri;
    node     *item;
    int      i, j;
    float temp_x, temp_y;
        
    list_for_each(&(obj->tri_list), item, i) {
        
        temp_tri = (triangle*)(item->payload);
        
        for(j = 0; j < 3; j++) {
            
            temp_x = temp_tri->v[j].x;
            temp_y = temp_tri->v[j].y;
            
            temp_tri->v[j].x = (temp_x * cos(rad_angle)) - (temp_y * sin(rad_angle));
            temp_tri->v[j].y = (temp_x * sin(rad_angle)) + (temp_y * cos(rad_angle));
        }
    }
}

void rotate_object_x_local(object* obj, float angle) {
    
    float oldx, oldy, oldz;
    
    oldx = obj->x;
    oldy = obj->y;
    oldz = obj->z;
    
    translate_object(obj, -oldx, -oldy, -oldz);
    rotate_object_x_global(obj, angle);
    translate_object(obj, oldx, oldy, oldz);
}

void rotate_object_y_local(object* obj, float angle) {
    
    float oldx, oldy, oldz;
    
    oldx = obj->x;
    oldy = obj->y;
    oldz = obj->z;
    
    translate_object(obj, -oldx, -oldy, -oldz);
    rotate_object_y_global(obj, angle);
    translate_object(obj, oldx, oldy, oldz);
}

void rotate_object_z_local(object* obj, float angle) {
    
    float oldx, oldy, oldz;
    
    oldx = obj->x;
    oldy = obj->y;
    oldz = obj->z;
    
    translate_object(obj, -oldx, -oldy, -oldz);
    rotate_object_z_global(obj, angle);
    translate_object(obj, oldx, oldy, oldz);
}

void project(vertex* v, screen_point* p) {

    float delta = (v->z == 0.0) ? 1.0 : (focal_length/v->z);

    p->x = TO_SCREEN_X(v->x * delta);
    p->y = TO_SCREEN_Y(v->y * delta);
    p->z = TO_SCREEN_Z(v->z);
}

void draw_triangle(triangle* tri) {
    
    int i;
    screen_point p[3];
    float vec_a[3];
    float vec_b[3];
    float cross[3];
    float mag;
    float normal_angle;
    float lighting_pct;
    float r, g, b;
    unsigned char f, s, t, e;
    float dx_1, dx_2, dx_3, dy_1, dy_2,	dy_3, dz_1, dz_2, dz_3;
    float mx_1, mx_2, mx_3, mz_1, mz_2, mz_3;
    float new_x1, new_x2, new_x3, new_z1, new_z2, new_z3;
    float first_orig_x, first_orig_y, first_orig_z, second_orig_x, second_orig_y, second_orig_z;
	float current_s;
    
    //Don't draw the triangle if it's offscreen
    if(tri->v[0].z < 0 && tri->v[1].z < 0 && tri->v[2].z < 0)
        return;
    
    //Calculate the surface normal
    //subtract 3 from 2 and 1, translating it to the origin
    vec_a[0] = tri->v[0].x - tri->v[2].x;
    vec_a[1] = tri->v[0].y - tri->v[2].y;
    vec_a[2] = tri->v[0].z - tri->v[2].z;
    vec_b[0] = tri->v[1].x - tri->v[2].x;
    vec_b[1] = tri->v[1].y - tri->v[2].y;
    vec_b[2] = tri->v[1].z - tri->v[2].z;
    
    //calculate the cross product using 1 as vector a and 2 as vector b
    cross[0] = vec_a[1]*vec_b[2] - vec_a[2]*vec_b[1];
    cross[1] = vec_a[2]*vec_b[0] - vec_a[0]*vec_b[2];
    cross[2] = vec_a[0]*vec_b[1] - vec_a[1]*vec_b[0]; 
    
    //normalize the result vector
    mag = sqrt(cross[0]*cross[0] + cross[1]*cross[1] + cross[2]*cross[2]);
    cross[0] /= mag;
    cross[1] /= mag;
    cross[2] /= mag;
        
    //Calculate the normal's angle vs the camera view direction
    normal_angle = acos(-cross[2]);
    
    //If the normal is facing away from the camera, don't bother drawing it
    if(normal_angle >= (3*PI/4)) {
        
        return;
    }
    
    //Calculate the shading color based on the first vertex color and the
    //angle between the camera and the surface normal
    lighting_pct = 1.0 - (normal_angle/PI);
    r = (float)tri->v[0].c->r * lighting_pct;
    r = r > 255.0 ? 255 : r;     
    g = (float)tri->v[0].c->g * lighting_pct;
    g = g > 255.0 ? 255 : g;
    b = (float)tri->v[0].c->b * lighting_pct;
    b = b > 255.0 ? 255 : b;
    //SDL_SetRenderDrawColor(rend, (unsigned char)r, (unsigned char)g, (unsigned char)b, 0xFF);
    
    //Move the vertices from world space to screen space
    for(i = 0; i < 3; i++) 
        project(&(tri->v[i]), &p[i]);
    
	S_draw_tri(p[0].x, p[0].y, p[1].x, p[1].y, p[2].x, p[2].y, (unsigned char)r & 0xFF, (unsigned char)g & 0xFF, (unsigned char)b * 0xFF);
	
	/* OLD LESTER WAY 
    //sort vertices by ascending y
    f = 0; s = 1; t = 2;
    if(p[f].y > p[s].y) {
        e = s;
        s = f;
        f = e;
    }
    if(p[s].y > p[t].y) {
        e = t;
        t = s;
        s = e;
    }
    if(p[f].y > p[s].y) {
        e = s;
        s = f;
        f = e;
    }
                   
	//Set the important scanlines
	current_s = p[f].y;
	dx_1 = p[s].x - p[f].x;
    dy_1 = p[s].y - p[f].y;
    dz_1 = p[s].z - p[f].z;
    dx_2 = p[t].x - p[s].x;
    dy_2 = p[t].y - p[s].y;
    dz_2 = p[t].z - p[s].z;
    dx_3 = p[t].x - p[f].x;
    dy_3 = p[t].y - p[f].y;
    dz_3 = p[t].z - p[f].z;
    mx_1 = dy_1 ? dx_1 / dy_1 : 0;
    mz_1 = dy_1 ? dz_1 / dy_1 : 0;
    mx_2 = dy_2 ? dx_2 / dy_2 : 0;
    mz_2 = dy_2 ? dz_2 / dy_2 : 0;
    mx_3 = dy_3 ? dx_3 / dy_3 : 0;
    mz_3 = dy_3 ? dz_3 / dy_3 : 0; 
    first_orig_x = p[f].x;
    first_orig_y = p[f].y;
    first_orig_z = p[f].z;
    second_orig_x = p[s].x;
    second_orig_y = p[s].y;
    second_orig_z = p[s].z;
	
	while(current_s < p[t].y) {
		
        if(!(current_s >= SCREEN_HEIGHT || current_s < 0)) {
            
            new_x3 = mx_3*(current_s - first_orig_y) + first_orig_x;
            new_z3 = mz_3*(current_s - first_orig_y) + first_orig_z;
            
            if(current_s < p[s].y) {
                
                new_x1 = mx_1*(current_s - first_orig_y) + first_orig_x;
                new_z1 = mz_1*(current_s - first_orig_y) + first_orig_z;
                
                //Draw the scanline from the first edge to the third 
                draw_scanline(rend, current_s, new_x1, new_z1, new_x3, new_z3);
            } else {
                
                new_x2 = mx_2*(current_s - second_orig_y) + second_orig_x;
                new_z2 = mz_2*(current_s - second_orig_y) + second_orig_z;
                
                //Draw the scanline from the second edge to the third 
                draw_scanline(rend, current_s, new_x2, new_z2, new_x3, new_z3);
            }
        }
           
		//Move to the next scanline		
		current_s += 1;
	}*/
}

void clip_and_render(triangle* tri) {    

    int count;
    int on_second_iteration = 0;
    int i;
    float plane_z = 0.2;
    float scale_factor, dx, dy, dz, ndz;
    unsigned char point_marked[3] = {0, 0, 0};
    vertex new_point[2]; 
    triangle out_triangle[2];
    int fixed[2];
    int original;
    
    
    //Note that in the future we're also going to need to clip on the
    //'U', 'V' and color axes
    while(1) {
        
        count = 0;
        
        //Check each point to see if it's greater than the plane
        for(i = 0; i < 3; i++) {
            
            if((on_second_iteration && tri->v[i].z > plane_z) || 
              (!on_second_iteration && tri->v[i].z < plane_z)) {

                point_marked[i] = 1;
                count++;
            } else {
                
                point_marked[i] = 0;
            }
        }
            
        switch(count) {
            
            //If all of the vertices were out of range, 
            //skip drawing the whole thing entirely
            case 3:
                return;
                break;
            
            //If one vertex was out, find it's edge intersections and
            //build two new triangles out of it
            case 1:
                //Figure out what the other two points are
                fixed[0] = point_marked[0] ? point_marked[1] ? 2 : 1 : 0;
                fixed[1] = fixed[0] == 0 ? point_marked[1] ? 2 : 1 : fixed[0] == 1 ? point_marked[0] ? 2 : 0 : point_marked[0] ? 1 : 0;
                original = point_marked[0] ? 0 : point_marked[1] ? 1 : 2;
                
                //Calculate the new intersection points
                for(i = 0; i < 2; i++) {
                    
                    //x,y, and z 'length'
                    dx = tri->v[original].x - tri->v[fixed[i]].x;
                    dy = tri->v[original].y - tri->v[fixed[i]].y;
                    dz = tri->v[original].z - tri->v[fixed[i]].z;
                                       
                    //Set the known axis value
                    new_point[i].z = plane_z; //Replace this with a line function
                    
                    //z 'length' of new point
                    ndz = new_point[i].z - tri->v[fixed[i]].z;
                    
                    //ratio of new y-length to to old
                    scale_factor = ndz/dz; //For now, we're dealing with a plane orthogonal to the clipping axis and as such 
                                           //we can't possibly have zero dy because that would place both the 'in' and 'out'
                                           //vertexes behind the plane, which is obviously impossible, so we won't worry about
                                           //that case until we start playing with sloped clipping planes
                    
                    //Scale the independent axis value by the scaling factor
                    //We can do this for other arbitrary axes in the future, such as U and V
                    new_point[i].x = scale_factor * dx + tri->v[fixed[i]].x;
                    new_point[i].y = scale_factor * dy + tri->v[fixed[i]].y;
                    
                    //Copy the color information
                    new_point[i].c = tri->v[fixed[i]].c;
                }   
                
                //Test/draw the new triangles, maintaining the CW or CCW ordering
                //Build the first triangle
                clone_vertex(&new_point[0], &(out_triangle[0].v[original]));
                clone_vertex(&(tri->v[fixed[0]]), &(out_triangle[0].v[fixed[0]]));
                clone_vertex(&(tri->v[fixed[1]]), &(out_triangle[0].v[fixed[1]]));
                    
                //Build the second triangle    
                clone_vertex(&new_point[1], &(out_triangle[1].v[original]));
                clone_vertex(&new_point[0], &(out_triangle[1].v[fixed[0]]));
                clone_vertex(&(tri->v[fixed[1]]), &(out_triangle[1].v[fixed[1]]));
                
                //Run the new triangles through another round of processing
                clip_and_render(&out_triangle[0]);
                clip_and_render(&out_triangle[1]);
                
                //Exit the function early for dat tail recursion              
                return;
                break;
            
            case 2:
                //Figure out which point we're keeping
                original = point_marked[0] ? point_marked[1] ? 2 : 1 : 0;
                fixed[0] = point_marked[0] ? 0 : point_marked[1] ? 1 : 2;
                fixed[1] = fixed[0] == 0 ? point_marked[1] ? 1 : 2 : fixed[0] == 1 ? point_marked[0] ? 0 : 2 : point_marked[0] ? 0 : 1;
                            
                //Calculate the new intersection points
                for(i = 0; i < 2; i++) {
                    
                    //x,y, and z 'length'
                    dx = tri->v[original].x - tri->v[fixed[i]].x;
                    dy = tri->v[original].y - tri->v[fixed[i]].y;
                    dz = tri->v[original].z - tri->v[fixed[i]].z;
                                       
                    //Set the known axis value
                    new_point[i].z = plane_z; //Replace this with a line function
                    
                    //z 'length' of new point
                    ndz = new_point[i].z - tri->v[fixed[i]].z;
                    
                    //ratio of new y-length to to old
                    scale_factor = ndz/dz; //For now, we're dealing with a plane orthogonal to the clipping axis and as such 
                                           //we can't possibly have zero dy because that would place both the 'in' and 'out'
                                           //vertexes behind the plane, which is obviously impossible, so we won't worry about
                                           //that case until we start playing with sloped clipping planes
                    
                    //Scale the independent axis value by the scaling factor
                    //We can do this for other arbitrary axes in the future, such as U and V
                    new_point[i].x = scale_factor * dx + tri->v[fixed[i]].x;
                    new_point[i].y = scale_factor * dy + tri->v[fixed[i]].y;
                    
                    //Copy the color information
                    new_point[i].c = tri->v[fixed[i]].c;
                }      
                
                //Start building the new triangles, maintaining the CW or CCW ordering 
                clone_vertex(&(tri->v[original]), &(out_triangle[0].v[original]));
                clone_vertex(&new_point[0], &(out_triangle[0].v[fixed[0]]));
                clone_vertex(&new_point[1], &(out_triangle[0].v[fixed[1]]));
                
                //Send through processing again
                clip_and_render(&out_triangle[0]);
                    
                //Exit the function early for dat tail recursion  
                return;
                break;
            
            //If there were no intersections we won't do anything and 
            //allow execution to flow through
            case 0:
            default:
                break; 
        }

        //If we hit case 0 both times above, all points on this
        //triangle lie in the drawable area and we can leave this
        //clipping loop and flow down to do the drawing of the 
        //processed triangle         
        if(on_second_iteration)
            break;
        
        on_second_iteration = 1;
        plane_z = SCREEN_DEPTH;
    }    
    
    //If we got this far, the triangle is drawable. So we should do that. Or whatever.
    draw_triangle(tri);   
}

void render_triangle(triangle* tri) {

    clip_and_render(tri);
}

void render_object(object *obj) {
    
    node* item;
    int i;
    
    list_for_each(&(obj->tri_list), item, i) {
        
        render_triangle((triangle*)item->payload);
    }
}

int main(int argc, char* argv[]) {

    int fov_angle, player_angle = 90, chg_angle = 0;
    float i = 0.0, step = 0, rstep = 0, fps, walkspeed = 0.04;
    color *c;
    object *cube1, *cube2;
    triangle test_tri[2];
    int done = 0;
    int numFrames = 0; 

    GPUinit();
	GPUopen(&disp, "Stationary", "./peops.cfg");
	//GPUdisplayText("You are now running Stationary");
	//updateDisplay();
	ulKeybits &= ~KEY_SHOWFPS;
	printf("You are now running stationary on display %lu\n", disp);
    printf("The GPU has the following status: 0x%08lx\n", (unsigned long)GPUreadStatus());
    S_do_gpu_startup();
    printf("The GPU has the following status: 0x%08lx\n", (unsigned long)GPUreadStatus());

    if(!(c = new_color(50, 200, 255, 255))) {
        
        printf("Could not allocate a new color\n");
        return -1;
    } 
    
    printf("Color created successfully\n");
    
    if(!(cube1 = new_cube(10.0, new_color(255, 255, 255, 255)))) {
        
        printf("Could not allocate a new cube\n");
        return -1;
    }
    
    if(!(cube2 = new_cube(1.0, c))) {
        
        printf("Could not allocate a new cube\n");
        return -1;
    }

    printf("Cube created successfully\n");

    fov_angle = 50;
    focal_length = 1.0 / (2.0 * tan(DEG_TO_RAD(fov_angle)/2.0));

    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    translate_object(cube1, 0.0, -6.0, 2.0);
    translate_object(cube2, 0.0, 0.0, 2.0);
    //rotate_object_y_local(cube, 45);
    //rotate_object_x_local(cube, 45);
    //rotate_object_z_local(cube, 45);
    
    /*
    test_tri[0].v[0].x = 0.5;
    test_tri[0].v[0].y = 0.5;
    test_tri[0].v[0].z = 1.0;
    test_tri[0].v[0].c = c;
    test_tri[0].v[1].x = 0.5;
    test_tri[0].v[1].y = -0.5;
    test_tri[0].v[1].z = 1.0;
    test_tri[0].v[1].c = c;
    test_tri[0].v[2].x = -0.5;
    test_tri[0].v[2].y = -0.5;
    test_tri[0].v[2].z = 1.0;
    test_tri[0].v[2].c = c;
    test_tri[1].v[0].x = 0.5;
    test_tri[1].v[0].y = 0.5;
    test_tri[1].v[0].z = 1.0;
    test_tri[1].v[0].c = c;
    test_tri[1].v[1].x = -0.5;
    test_tri[1].v[1].y = -0.5;
    test_tri[1].v[1].z = 1.0;
    test_tri[1].v[1].c = c;
    test_tri[1].v[2].x = -0.5;
    test_tri[1].v[2].y = 0.5;
    test_tri[1].v[2].z = 1.0;
    test_tri[1].v[2].c = c;
    */

    while(!done) {

        i += step;
        //translate_object(cube1, 0.0, 0.0, step);
        rotate_object_y_local(cube1, 1);
        rotate_object_x_local(cube1, 1);
        rotate_object_z_local(cube1, 1);        
        rotate_object_x_local(cube2, 1);
        rotate_object_z_local(cube2, 1);

        S_clear_framebuffer(0xFF00);
        
        render_object(cube1);
        render_object(cube2);  
        //render_triangle(renderer, &test_tri[0]);
        //render_triangle(renderer, &test_tri[1]);
        
        updateDisplay();
        
        //while((SDL_GetTicks() - frame_start) <= 14);
    }

    return 0;
}

int old_main(int argc, char* argv[]) {
	
	unsigned long disp;
    unsigned long status;
    unsigned short tip_x;
    int direction;
	
	GPUinit();
	GPUopen(&disp, "Stationary", "./peops.cfg");
	//GPUdisplayText("You are now running Stationary");
	//updateDisplay();
	ulKeybits &= ~KEY_SHOWFPS;
	printf("You are now running stationary on display %lu\n", disp);
    printf("The GPU has the following status: 0x%08lx\n", (unsigned long)GPUreadStatus());
    S_do_gpu_startup();
    printf("The GPU has the following status: 0x%08lx\n", (unsigned long)GPUreadStatus());
	S_clear_framebuffer(0xFF00);
    
    tip_x = 0;
    direction = 1;
    
    while(1) {
     
        S_draw_tri(0, 0, 320, 240, 0, 240, 0xFF, 0x00, 0x00);
        S_draw_tri(0, 0, 320, 0, 320, 240, 0x00, 0xFF, 0x00);
        S_draw_tri_textured(0, 240, tip_x, 0, 320, 240, 0xFF, 0xFF, 0xFF);
        updateDisplay();
        
        if(direction) {
            
            if(tip_x == 320) 
                direction = 0;
            else
                tip_x++;
        } else {
            
            if(tip_x == 0) 
                direction = 1;
            else
                tip_x--;
        }
    } 
    
	return 1;
}