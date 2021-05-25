#!/bin/bash
#-------------------------------------------------------------------------#
# STATIONARY COMPONENTS                                                   #
#-------------------------------------------------------------------------#
emcc -c -o stationary_test_main.bc stationary_test_main.c -g
emcc -c -o list.bc list.c -g
emcc -c -o vertex.bc vertex.c -g
emcc -c -o triangle.bc triangle.c -g
emcc -c -o object.bc object.c -g
emcc -c -o cube.bc cube.c -g
emcc -c -o screentriangle.bc screentriangle.c -g 
emcc -c -o zlist.bc zlist.c -g
emcc -c -o renderer.bc renderer.c -g

#-------------------------------------------------------------------------#
# PEOPS COMPONENTS                                                        #
#-------------------------------------------------------------------------#
emcc -c -o cfg.bc peops/cfg.c -g
emcc -c -o draw.bc peops/draw.c -g
emcc -c -o DrawString.bc peops/DrawString.c -g 
emcc -c -o fps.bc peops/fps.c -g
emcc -c -o gpu.bc peops/gpu.c -g
emcc -c -o key.bc peops/key.c -g
emcc -c -o menu.bc peops/menu.c -g
emcc -c -o prim.bc peops/prim.c -g
emcc -c -o soft.bc peops/soft.c -g

#-------------------------------------------------------------------------#
# EMSCRIPTEN SHIM                                                         #
#-------------------------------------------------------------------------#
emcc -c -o webshim.bc webshim.c -g

#-------------------------------------------------------------------------#
# LINK                                                                    #
#-------------------------------------------------------------------------#
emcc -o stationary_test.js *.bc -lm -g -s NO_EXIT_RUNTIME=1 -s EXPORTED_FUNCTIONS='["_do_render_loop_proc","_main"]' -s LEGACY_VM_SUPPORT=1 -s "EXTRA_EXPORTED_RUNTIME_METHODS=['ccall','cwrap']"
scp {index.html,stationary_test.js,stationary_test.wasm} jmarlin@192.168.1.210:/usr/local/www/stationary/
#TODO: Switch over to a makefile
