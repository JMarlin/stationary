rem -------------------------------------------------------------------------#
rem  STATIONARY COMPONENTS                                                   #
rem -------------------------------------------------------------------------#
call emcc -c -o stationary_test_main.bc stationary_test_main.c -g
call emcc -c -o list.bc list.c -g
call emcc -c -o vertex.bc vertex.c -g
call emcc -c -o triangle.bc triangle.c -g
call emcc -c -o object.bc object.c -g
call emcc -c -o cube.bc cube.c -g
call emcc -c -o screentriangle.bc screentriangle.c -g 
call emcc -c -o zlist.bc zlist.c -g
call emcc -c -o renderer.bc renderer.c -g

rem -------------------------------------------------------------------------#
rem  PEOPS COMPONENTS                                                        #
rem -------------------------------------------------------------------------#
call emcc -c -o cfg.bc peops/cfg.c -g
call emcc -c -o draw.bc peops/draw.c -g
call emcc -c -o DrawString.bc peops/DrawString.c -g 
call emcc -c -o fps.bc peops/fps.c -g
call emcc -c -o gpu.bc peops/gpu.c -g
call emcc -c -o key.bc peops/key.c -g
call emcc -c -o menu.bc peops/menu.c -g
call emcc -c -o prim.bc peops/prim.c -g
call emcc -c -o soft.bc peops/soft.c -g

rem -------------------------------------------------------------------------#
rem  EMSCRIPTEN SHIM                                                         #
rem -------------------------------------------------------------------------#
call emcc -c -o webshim.bc webshim.c -g

rem -------------------------------------------------------------------------#
rem  LINK                                                                    #
rem -------------------------------------------------------------------------#
call emcc -o stationary_test.js cfg.bc cube.bc draw.bc DrawString.bc fps.bc gpu.bc key.bc list.bc menu.bc object.bc prim.bc renderer.bc screentriangle.bc soft.bc stationary_test_main.bc triangle.bc vertex.bc webshim.bc zlist.bc zn.bc -lm -g -s NO_EXIT_RUNTIME=1 -s EXPORTED_FUNCTIONS=["_do_render_loop_proc","_do_event_proc","_main"] -s LEGACY_VM_SUPPORT=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=["ccall","cwrap"]
