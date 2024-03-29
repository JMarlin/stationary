#include "renderer.h"
#include <math.h>
#include <inttypes.h>
#include "cube.h"
#include "vertex.h"
#include "color.h"
#include "triangle.h"
#include "object.h"
#include "list.h"
#include "zlist.h"
#include "peops/externals.h"
#include "peops/gpu.h"
#include "mathhelpers.h"
#include "screentriangle.h"

float focal_length = 0;

void S_set_fov_angle(int fov_angle) {
	
	focal_length = 1.0 / (2.0 * tan(DEG_TO_RAD(fov_angle)/2.0));
}

int S_do_gpu_startup() {
	
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
	
	return ZList_init();
}

void S_draw_tri(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t r, uint8_t g, uint8_t b) {
    
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

void S_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t r, uint8_t g, uint8_t b) {

	//Line, one color
	GPUwriteData(
		(GPU_DATA_CMD(PRIM_LINE, 0) << 24) |
		b << 16 |
		g << 8 |
		r 
	);
	    
	//Vertex 1
	GPUwriteData((y0 << 16) | x0);
	
 	//Vertex 2
    	GPUwriteData((y1 << 16) | x1);
}

void S_upload_image_data(uint32_t* src, uint16_t x, uint16_t y, uint16_t h, uint16_t w) {
	
	int pixel_count = (h * w) >> 1;
	int i;
	
	GPUwriteData(0x01000000); //Reset the command buffer
	GPUwriteData(0xA0000000); //Copy image data to GPU command
	GPUwriteData((y << 16) | x); //Send x and y of destination
	GPUwriteData((h << 16) | w); //Send h and w of image
	
	for(i = 0; i < pixel_count; i++)
	    GPUwriteData(src[i]);
}

void S_clear_framebuffer(uint16_t val) {
	
	int pixel_count = (1024 * 512) >> 1;
	int i;
	
	GPUwriteData(0x01000000); //Reset the command buffer
	GPUwriteData(0xA0000000); //Copy image data to GPU command
	GPUwriteData(0x00000000); //Send x and y of destination
	GPUwriteData((512 << 16) | 1024); //Send h and w of image
	
	for(i = 0; i < pixel_count; i++)
	    GPUwriteData((val << 16) | val);
}

void S_fill_background(uint8_t r, uint8_t g, uint8_t b) {

    #warning S_fill_background hardcoded to 320x240 mode
    S_draw_tri(0, 0, 320, 0, 320, 240, r, g, b);
    S_draw_tri(0, 0, 320, 240, 0, 240, r, g, b);
}

Texture S_load_texture(
    uint32_t* src,
    uint16_t x, uint16_t y,
    uint8_t w, uint8_t h
) {

    Texture ret_texture;
	
	S_upload_image_data(src, x, y, w, h);

    //Texture page is 5 bits
    //y=(one bit)(x256) x=(four bits)(x64)

    uint8_t tp_x = (uint8_t)((x >> 6) & 0x0F);
    uint8_t tp_y = (uint8_t)(y > 0xFF ? 1 : 0);

    ret_texture.texture_page = (tp_y << 4) | tp_x;
    ret_texture.x = (uint8_t)((x - (tp_x << 6)) & 0xFF);
    ret_texture.y = (uint8_t)(y & 0xFF);
    ret_texture.w = w;
    ret_texture.h = h;

    return ret_texture;
}

void S_draw_tri_textured(
    uint16_t x0, uint16_t y0,
    uint16_t x1, uint16_t y1,
    uint16_t x2, uint16_t y2,
    Texture texture,
    color24 c,
    uint8_t v0, uint8_t u0,
    uint8_t v1, uint8_t u1,
    uint8_t v2, uint8_t u2
) {
    		
    //Poly, one color, flat shaded
    GPUwriteData((GPU_DATA_CMD(PRIM_POLY, OPT_TME) << 24) | c);
    
    //Vertex 1
    GPUwriteData((y0 << 16) | x0);
    
	//Clut ID and texture location 1
	GPUwriteData(
        (0x0000 << 16) | //no CLUT
        ((uint32_t)v0 << 8)    | 
        ((uint32_t)u0 << 0)      
    );
	
    //Vertex 2
    GPUwriteData((y1 << 16) | x1);

    //01 10    08 00
    //0000 0001 0001 0000
    uint32_t tp_data =
        //7 bits of emptiness
        (0x8 << (16 + 5)) | //color_mode=10b (2=15-bit, 1=8bit, 0=4-bit), semi_trans=00b
        (((uint32_t)texture.texture_page & 0x1f) << 16) | //y=(one bit)(x256) x=(four bits)(x64)
        ((uint32_t)v1 << 8) |
        ((uint32_t)u1 << 0);
    
	//Texture page info and second texture location
	GPUwriteData(
        tp_data
    ); 
	
    //Vertex 3
    GPUwriteData((y2 << 16) | x2);
	
	//Third texture location
	GPUwriteData(
        //16 bits of emptiness
        ((uint32_t)v2 << 8) |
        ((uint32_t)u2 << 0)
    );
}

void S_draw_quad_textured(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, color24 c) {
    
	//Make sure our texture is in the vram
	static uint32_t texture[] = {
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
    GPUwriteData((0x2C << 24) | c);
    
    //Vertex 1
    GPUwriteData((y0 << 16) | x0);
    
	//Clut ID and texture location 1
	GPUwriteData(0x00000000); //no CLUT, v = 0, u = 0
	
    //Vertex 2
    GPUwriteData((y1 << 16) | x1);
    
	//Texture page info and second texture location
	GPUwriteData(0x01900008); //Use 15-bit direct texture at (0,256) -- v = 0, u = 8 
	
    //Vertex 3
    GPUwriteData((y2 << 16) | x2);
	
	//Third texture location
	GPUwriteData(0x00000808); //v = 8, u = 8
	
	//Vertex 4
    GPUwriteData((y3 << 16) | x3);
	
	//Third texture location
	GPUwriteData(0x00000800); //v = 8, u = 0
}

void project(Vertex* v, ScreenPoint* p) {

    float delta = (v->z == 0.0) ? 1.0 : (focal_length/v->z);

    p->x = TO_SCREEN_X(v->x * delta);
    p->y = TO_SCREEN_Y(v->y * delta);
    p->z = v->z;
    p->u = v->u;
    p->v = v->v;
}

int is_backface(ScreenPoint* a, ScreenPoint* b, ScreenPoint* c) {

    float ax = ((float)a->x) - ((float)b->x);
    float ay = ((float)a->y) - ((float)b->y);
    float bx = ((float)a->x) - ((float)c->x);
    float by = ((float)a->y) - ((float)c->y);

    float cz = ax * by - ay * bx;

    return cz < 0;
}

void draw_triangle(Triangle* triangle) {
    
    int i;
    ScreenPoint p[3];
    float vec_a[3];
    float vec_b[3];
    float cross[3];
    float mag;
    float normal_angle;
    float lighting_pct;
    float r, g, b;
    uint8_t f, s, t, e;
    float dx_1, dx_2, dx_3, dy_1, dy_2,	dy_3, dz_1, dz_2, dz_3;
    float mx_1, mx_2, mx_3, mz_1, mz_2, mz_3;
    float new_x1, new_x2, new_x3, new_z1, new_z2, new_z3;
    float first_orig_x, first_orig_y, first_orig_z, second_orig_x, second_orig_y, second_orig_z;
	float current_s;
    
    //Don't draw the triangle if it's offscreen
    if(triangle->v[0].z < 0 && triangle->v[1].z < 0 && triangle->v[2].z < 0)
        return;
    /*
    //Calculate the surface normal
    //subtract 3 from 2 and 1, translating it to the origin
    vec_a[0] = triangle->v[0].x - triangle->v[2].x;
    vec_a[1] = triangle->v[0].y - triangle->v[2].y;
    vec_a[2] = triangle->v[0].z - triangle->v[2].z;
    vec_b[0] = triangle->v[1].x - triangle->v[2].x;
    vec_b[1] = triangle->v[1].y - triangle->v[2].y;
    vec_b[2] = triangle->v[1].z - triangle->v[2].z;
    
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
    
	if(normal_angle > (PI/2)) {
        
        return;
    }    

    //Calculate the shading color based on the first vertex color and the
    //angle between the camera and the surface normal
    lighting_pct = 1.0 - (normal_angle/PI);
    r = (float)RGB24_RED(triangle->v[0].c) * lighting_pct;
    r = r > 255.0 ? 255 : r;     
    g = (float)RGB24_GRN(triangle->v[0].c) * lighting_pct;
    g = g > 255.0 ? 255 : g;
    b = (float)RGB24_BLU(triangle->v[0].c) * lighting_pct;
    b = b > 255.0 ? 255 : b;
    */
    
    //Move the vertices from world space to screen space
    for(i = 0; i < 3; i++) 
        project(&(triangle->v[i]), &p[i]);
    
    if(is_backface(&p[0], &p[1], &p[2])) {

        return;
    }

	//S_draw_tri(p[0].x, p[0].y, p[1].x, p[1].y, p[2].x, p[2].y, (uint8_t)r & 0xFF, (uint8_t)g & 0xFF, (uint8_t)b * 0xFF);
	ZList_add(&p[0], &p[1], &p[2], triangle->c, triangle->tex);
}

void clip_and_render(Triangle* triangle) {    

    int count;
    int on_second_iteration = 0;
    int i;
    float plane_z = 0.2;
    float scale_factor, dx, dy, dz, ndz;
    uint8_t point_marked[3] = {0, 0, 0};
    Vertex new_point[2]; 
    Triangle out_triangle[2];
    int fixed[2];
    int original;
    
    
    //Note that in the future we're also going to need to clip on the
    //'U', 'V' and color axes
    while(1) {
        
        count = 0;
        
        //Check each point to see if it's greater than the plane
        for(i = 0; i < 3; i++) {
            
            if((on_second_iteration && triangle->v[i].z > plane_z) || 
              (!on_second_iteration && triangle->v[i].z < plane_z)) {

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
                    dx = triangle->v[original].x - triangle->v[fixed[i]].x;
                    dy = triangle->v[original].y - triangle->v[fixed[i]].y;
                    dz = triangle->v[original].z - triangle->v[fixed[i]].z;
                                       
                    //Set the known axis value
                    new_point[i].z = plane_z; //Replace this with a line function
                    
                    //z 'length' of new point
                    ndz = new_point[i].z - triangle->v[fixed[i]].z;
                    
                    //ratio of new y-length to to old
                    scale_factor = ndz/dz; //For now, we're dealing with a plane orthogonal to the clipping axis and as such 
                                           //we can't possibly have zero dy because that would place both the 'in' and 'out'
                                           //vertexes behind the plane, which is obviously impossible, so we won't worry about
                                           //that case until we start playing with sloped clipping planes
                    
                    //Scale the independent axis value by the scaling factor
                    //We can do this for other arbitrary axes in the future, such as U and V
                    new_point[i].x = scale_factor * dx + triangle->v[fixed[i]].x;
                    new_point[i].y = scale_factor * dy + triangle->v[fixed[i]].y;
                    
                    //Copy the color information
                    new_point[i].c = triangle->v[fixed[i]].c;
                }   
                
                //Test/draw the new triangles, maintaining the CW or CCW ordering
                //Build the first triangle
                Vertex_clone(&new_point[0], &(out_triangle[0].v[original]));
                Vertex_clone(&(triangle->v[fixed[0]]), &(out_triangle[0].v[fixed[0]]));
                Vertex_clone(&(triangle->v[fixed[1]]), &(out_triangle[0].v[fixed[1]]));
                    
                //Build the second triangle    
                Vertex_clone(&new_point[1], &(out_triangle[1].v[original]));
                Vertex_clone(&new_point[0], &(out_triangle[1].v[fixed[0]]));
                Vertex_clone(&(triangle->v[fixed[1]]), &(out_triangle[1].v[fixed[1]]));
                
                //Run the new triangles through another round of processing
				out_triangle[0].c = triangle->c;
				out_triangle[1].c = triangle->c;
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
                    dx = triangle->v[original].x - triangle->v[fixed[i]].x;
                    dy = triangle->v[original].y - triangle->v[fixed[i]].y;
                    dz = triangle->v[original].z - triangle->v[fixed[i]].z;
                                       
                    //Set the known axis value
                    new_point[i].z = plane_z; //Replace this with a line function
                    
                    //z 'length' of new point
                    ndz = new_point[i].z - triangle->v[fixed[i]].z;
                    
                    //ratio of new y-length to to old
                    scale_factor = ndz/dz; //For now, we're dealing with a plane orthogonal to the clipping axis and as such 
                                           //we can't possibly have zero dy because that would place both the 'in' and 'out'
                                           //vertexes behind the plane, which is obviously impossible, so we won't worry about
                                           //that case until we start playing with sloped clipping planes
                    
                    //Scale the independent axis value by the scaling factor
                    //We can do this for other arbitrary axes in the future, such as U and V
                    new_point[i].x = scale_factor * dx + triangle->v[fixed[i]].x;
                    new_point[i].y = scale_factor * dy + triangle->v[fixed[i]].y;
                    
                    //Copy the color information
                    new_point[i].c = triangle->v[fixed[i]].c;
                }      
                
                //Start building the new triangles, maintaining the CW or CCW ordering 
                Vertex_clone(&(triangle->v[original]), &(out_triangle[0].v[original]));
                Vertex_clone(&new_point[0], &(out_triangle[0].v[fixed[0]]));
                Vertex_clone(&new_point[1], &(out_triangle[0].v[fixed[1]]));
                
                //Send through processing again
				out_triangle[0].c = triangle->c;
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
    draw_triangle(triangle);   
}

void Triangle_render(Triangle* triangle) {

    clip_and_render(triangle);
}

void Object_render(Object *object) {
    
    Triangle *triangle;
    
    List_for_each(object->triangles, triangle, Triangle*)        
        Triangle_render(triangle);
}

void S_commit_scene() {
	
	ZList_render();
	ZList_clear();
}
