#include "webshim.h"
#include <emscripten.h>
#include <stdlib.h>

EventDispatchProcedure g_event_proc = 0;

int last_mouse_x = 0;
int last_mouse_y = 0;

void WS_InitEvents() {

    #warning Not Implemented: WS_InitEvents
}

RenderProcedure g_render_proc = 0;

void do_render_loop_proc() {

    g_render_proc();
}

void do_event_proc(int x, int y) {
	last_mouse_x = x;
	last_mouse_y = y;
	
	g_event_proc();
}

void WS_GetMouse(int* x, int* y) {
	*x = last_mouse_x;
	*y = last_mouse_y;
}

void WS_SetRenderLoopProc(RenderProcedure render_proc) {

    g_render_proc = render_proc;

    EM_ASM_(
        Module.do_render_proc = Module.cwrap('do_render_loop_proc');
    );
}

void WS_StartRenderLoop() {

     EM_ASM_(

         if(!!Module.render_interval) {

             return;
         }

        //Start render loop
        Module.render_interval = setInterval(function() {

            Module.do_render_proc();
        }, 0);
     );
}

void WS_StartEventDispatch(EventDispatchProcedure dispatch_proc) {

    g_event_proc = dispatch_proc;

    EM_ASM_(
        Module.do_event_proc = Module.cwrap('do_event_proc', null, ['number', 'number']);
    );

    //TODO: Dispatch browser events to the dispatch_proc
    //      shutdown application if dispatch_proc returns 1
    #warning Not Implemented: WS_StartEventDispatch
    #warning Not Implemented: Event type needs to be created and passed to WS_StartEventDispatch
}

WS_Display* WS_CreateDisplay(uint32_t w, uint32_t h) {

    WS_Display* display = (WS_Display*)malloc(sizeof(WS_Display));

    #warning Not error handled: display malloc

    display->w = w;
    display->h = h;
    display->id = EM_ASM_INT({

        if(!Module.display_list) {

            Module.display_list = [];
        }

        return Module.display_list.length
    }, 0);

    EM_ASM_({

        var new_canvas = document.createElement('canvas');

        document.body.style.margin = '0px';
        document.body.style.backgroundColor = '#000';
        
        var resizer = (e) => {

            var display_ratio = 320.0/240.0;

            var window_ratio = 
                window.innerWidth / window.innerHeight;
            
            if(display_ratio < window_ratio) {
                
                new_canvas.style.top = "0px";

                new_canvas.style.height = 
                    "" + window.innerHeight + "px";

                var canvas_width =
                    (window.innerHeight * display_ratio);

                new_canvas.style.width =
                    "" + canvas_width + "px";

                new_canvas.style.left =
                    "" + ((window.innerWidth - canvas_width) / 2) + "px";
            } else {

                new_canvas.style.left = "0px";

                new_canvas.style.width = 
                    "" + window.innerWidth + "px";

                var canvas_height = 
                    (window.innerWidth / display_ratio);

                new_canvas.style.height =
                    "" + canvas_height + "px";

                new_canvas.style.top =
                    "" + ((window.innerHeight - canvas_height) / 2) + "px";
            }
        };

        window.addEventListener('resize', resizer);
	new_canvas.addEventListener('mousemove', e => {
		const scale = 320 / parseInt(new_canvas.style.width);
		const new_coords = {
		    x: e.offsetX * scale,
		    y: e.offsetY * scale
		};
		
		Module.do_event_proc(new_coords.x, new_coords.y);
	});

        new_canvas.style.cursor = 'none';
        new_canvas.style.position = 'absolute';
        new_canvas.width = $0;
        new_canvas.height = $1;
        
        document.body.appendChild(new_canvas);
        
        Module.display_list.push({
            canvas: new_canvas,
            framebuf: new Uint8Array(320 * 240 * 4)
        });

        resizer(null);

    }, display->w, display->h);

    return display;
}

WS_ImgBuf* WS_CreateImgBuf(uint32_t w, uint32_t h, uint32_t depth) {
    
    WS_ImgBuf* image_buf = (WS_ImgBuf*)malloc(sizeof(WS_ImgBuf));

    #warning Not error handled: img buf malloc

    image_buf->data = (unsigned char*)malloc(
        w * h * (depth == IMG_BUF_DEPTH_16 ? 2 : 3)
    );

    #warning Not error handled: img buf data malloc

    image_buf->w = w;
    image_buf->h = h;
    image_buf->depth = depth;
    image_buf->owns_data = 1;

    return image_buf;
}

WS_ImgBuf* WS_CreateImgBufFrom(void* buf, uint32_t w, uint32_t h, uint32_t depth) {
    
    WS_ImgBuf* image_buf = (WS_ImgBuf*)malloc(sizeof(WS_ImgBuf));

    #warning Not error handled: img buf malloc

    image_buf->data = (unsigned char*)buf;

    image_buf->w = w;
    image_buf->h = h;
    image_buf->depth = depth;
    image_buf->owns_data = 0;

    return image_buf;
}

void WS_DestroyImgBuf(WS_ImgBuf* img_buf) {

    if(img_buf->owns_data) {

        free((void*)img_buf->data);
    }

    free(img_buf);
}

void WS_DestroyDisplay(WS_Display* display) {

    #warning Not Implemented: WS_DestroyDisplay
}

void WS_ClearDisplay(WS_Display* display) {

    #warning Not Implemented: WS_ClearDisplay
}

void WS_StopRenderLoop() {

    EM_ASM_(

        if(!!Module.render_interval) {

            clearInterval(Module.render_interval);
            Module.render_interval = null;
        }
    );
}

void WS_BlitToDisplay(WS_Display* display, WS_ImgBuf* img_buf, WS_Rect* rectsrc, WS_Rect* rectdst) {

    EM_ASM({

        var display_id = $0;
        var display_w = $1;
        var display_h = $2;
        var src_w = $3;
        var src_h = $4;
        var src_addr = $5;
        var src_depth = $6;

        var disp_data = Module.display_list[display_id];
        var canvas = disp_data.canvas;
        var fb = disp_data.framebuf;
        var ctx = canvas.getContext('2d');
        var canv_data = ctx.getImageData(0, 0, display_w, display_h);

        //Super slow land -- first copy into the backing fb
        //while converting the pixel format on the fly, then
        //shove the backing fb into the canvas
        #warning This does not take into acount the src and dest rects
        #warning For starters, the width of the vram 'image' is much wider than the 'display' window
        var i = 0;
        var src_view = Module.HEAPU8.subarray(
            src_addr, src_addr + (src_w * src_h * (src_depth === 0 ? 2 : 3))
        );

        var i = 0;
        if(src_depth === 0) {
            for(var x = 0; x < display_w; x++) {
                for(var y = 0; y < display_h; y++) {
                    var px0 = src_view[y * 2 * src_w + x * 2 + 0];
                    var px1 = src_view[y * 2 * src_w + x * 2 + 1];
                    var r = px0 & 0x1F;
                    var g = ((px1 & 0x03) << 3) + ((px0 & 0xE0)>>5);
                    var b = (px1 & 0x7C) >> 2;                    
                    fb[y * 4 * display_w + x * 4 + 0] = (r << 3) + (r & 0x07);
                    fb[y * 4 * display_w + x * 4 + 1] = (g << 3) + (g & 0x07);
                    fb[y * 4 * display_w + x * 4 + 2] = (b << 3) + (b & 0x07);
                    fb[y * 4 * display_w + x * 4 + 3] = 0xFF;
                }
            }
        } else {
            for(var x = 0; x < display_w; x++) {
                for(var y = 0; y < display_h; y++) {
                    fb[y * 4 * display_w + x * 4 + 0] = src_view[y * 3 * src_w + x * 3 + 0];
                    fb[y * 4 * display_w + x * 4 + 1] = src_view[y * 3 * src_w + x * 3 + 1];
                    fb[y * 4 * display_w + x * 4 + 2] = src_view[y * 3 * src_w + x * 3 + 2];
                    fb[y * 4 * display_w + x * 4 + 3] = 0xFF;
                }
            }
        }

        //Create an unsigned byte subarray  
        canv_data.data.set(fb); 
        ctx.putImageData(canv_data, 0, 0);
    },
        display->id, display->w, display->h,
        img_buf->w, img_buf->h, img_buf->data, img_buf->depth
    );
}

void WS_UpdateDisplay(WS_Display* display) {

    #warning Not Implemented: WS_UpdateDisplay
}
