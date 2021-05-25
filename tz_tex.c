#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "gimp_input.h"

const char* header_name = "TZ_TEXT_H";
const char* h_out = "tz_text.h";
const char* var_name = "tz_text";

int main(int argc, char* argv) {

  FILE* out_file = fopen(h_out, "wb");

  fprintf(out_file, "#ifndef %s\n", header_name);
  fprintf(out_file, "#define %s\n\n", header_name);
  fprintf(out_file, "#include <inttypes.h>\n\n");
  fprintf(out_file, "uint16_t %s[] = {\n    ", var_name);

  int outs = 0;

  for(int i = 0; i < (gimp_image.height * gimp_image.width * 3); i += 3) {

    unsigned char r = gimp_image.pixel_data[i + 0];
    unsigned char g = gimp_image.pixel_data[i + 1];
    unsigned char b = gimp_image.pixel_data[i + 2];

    uint16_t out_pix = 0x8000 |
      ((((uint16_t)r & 0xF8) >> 4) << 10) |
      ((((uint16_t)g & 0xF8) >> 4) << 5) |
      ((((uint16_t)b & 0xF8) >> 4) << 0);

    fprintf(out_file, "0x%04X,%s", out_pix, ((outs % gimp_image.width) == (gimp_image.width - 1)) ? "\n    " : "");
    
    outs++;
  }

  fprintf(out_file, "\n};\n#endif //%s\n", header_name);

  fclose(out_file);
}