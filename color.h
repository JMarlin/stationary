#ifndef COLOR_H
#define COLOR_H

typedef unsigned short color15;
typedef unsigned long color24;

#define COLOR_CLIP15(c) (((c) > 0x1F) ? 0x1F : (c))
#define RGB15(r, g, b) ((0x8000) | (COLOR_CLIP15(b) << 10) | (COLOR_CLIP15(g) << 5) | COLOR_CLIP15(r))
#define RGB15_RED(c) (unsigned char)((c) & 0x1F);
#define RGB15_GRN(c) (unsigned char)(((c) >> 5) & 0x1F);
#define RGB15_BLU(c) (unsigned char)(((c) >> 10) & 0x1F);

#define COLOR_CLIP24(c) (((c) > 0xFF) ? 0xFF : (c))
#define RGB24(r, g, b) (color24)((COLOR_CLIP24(b) << 16) | (COLOR_CLIP24(g) << 8) | COLOR_CLIP24(r))
#define RGB24_RED(c) (unsigned char)((c) & 0xFF);
#define RGB24_GRN(c) (unsigned char)(((c) >> 8) & 0xFF);
#define RGB24_BLU(c) (unsigned char)(((c) >> 16) & 0xFF);

#endif //COLOR_H