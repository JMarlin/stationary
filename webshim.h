#ifndef WEBSHIM_H
#define WEBSHIM_H

#include <inttypes.h>

typedef void (*RenderProcedure)();
typedef int (*EventDispatchProcedure)();

typedef struct S_WS_Rect {
    int32_t x;
    int32_t y;
    int32_t w;
    int32_t h;
} WS_Rect;

#define IMG_BUF_DEPTH_16 0
#define IMG_BUF_DEPTH_24 1

typedef struct S_WS_ImgBuf {
    int32_t w;
    int32_t h;
    int32_t depth;
    int32_t owns_data;
    unsigned char* data;
} WS_ImgBuf;

typedef struct S_WS_Display {
    int32_t w;
    int32_t h;
    int id;
} WS_Display;

void WS_InitEvents();
void WS_SetRenderLoopProc(RenderProcedure render_proc);
void WS_StopRenderLoop();
void WS_StartRenderLoop();
void WS_StartEventDispatch(EventDispatchProcedure dispatch_proc);
WS_ImgBuf* WS_CreateImgBuf(uint32_t w, uint32_t h, uint32_t depth);
WS_ImgBuf* WS_CreateImgBufFrom(void* buf, uint32_t w, uint32_t h, uint32_t depth);
void WS_DestroyImgBuf(WS_ImgBuf* img_buf);
WS_Display* WS_CreateDisplay(uint32_t w, uint32_t h);
void WS_DestroyDisplay(WS_Display* display);
void WS_BlitToDisplay(WS_Display* display, WS_ImgBuf* img_buf, WS_Rect* rectsrc, WS_Rect* rectdst);
void WS_ClearDisplay(WS_Display* display);
void WS_UpdateDisplay(WS_Display* display);
void WS_GetMouse(int* x, int* y, int* buttons);

#endif //WEBSHIM_H
