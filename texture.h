#ifndef TEXTURE_H
#define TEXTURE_H

#include <inttypes.h>

typedef struct S_Texture {
    uint16_t texture_page;
    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t h;
    #warning TODO: Further texture details (format &c)
} Texture;

#endif