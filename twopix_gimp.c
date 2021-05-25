/* GIMP RGB C-Source image dump (twopix_gimp.c) */

static const struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[2 * 2 * 3 + 1];
} gimp_image = {
  2, 2, 3,
  "\000\000\000\377\377\377\377\377\377\000\000\000",
};

