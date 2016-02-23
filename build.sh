#!/bin/sh
#-------------------------------------------------------------------------#
# STATIONARY COMPONENTS                                                   #
#-------------------------------------------------------------------------#
gcc -c -o stationary_test_main.o stationary_test_main.c -m32 -g
gcc -c -o list.o list.c -m32 -g
gcc -c -o vertex.o vertex.c -m32 -g
gcc -c -o triangle.o triangle.c -m32 -g
gcc -c -o object.o object.c -m32 -g
gcc -c -o cube.o cube.c -m32 -g
gcc -c -o screentriangle.o screentriangle.c -m32 -g 
gcc -c -o zlist.o zlist.c -m32 -g
gcc -c -o renderer.o renderer.c -m32 -g

#-------------------------------------------------------------------------#
# PEOPS COMPONENTS                                                        #
#-------------------------------------------------------------------------#
#gcc -c `pkg-config --cflags gtk+-3.0` -o callbacks.o  peops/callbacks.c `pkg-config --libs gtk+-3.0` -m32 -g 
gcc -c -o cfg.o peops/cfg.c $(sdl2-config --cflags) -m32 -g
#gcc -c -o conf.o peops/conf.c #IGNORING GTK COMPONENT
gcc -c -o draw.o peops/draw.c $(sdl2-config --cflags) -m32 -g
gcc -c -o DrawString.o peops/DrawString.c -m32 -g 
gcc -c -o fps.o peops/fps.c $(sdl2-config --cflags) -m32 -g
#gcc -c -o fpsewp.o  peops/fpsewp.c -I ./peops/fpse -m32
gcc -c -o gpu.o peops/gpu.c $(sdl2-config --cflags) -m32 -g
nasm -o hq2x16.o -felf32 peops/hq2x16.asm -I ./peops/ -w-all
nasm -o hq2x32.o -felf32 peops/hq2x32.asm -I ./peops/ -w-all
nasm -o hq3x16.o -felf32 peops/hq3x16.asm -I ./peops/ -w-all
nasm -o hq3x32.o -felf32 peops/hq3x32.asm -I ./peops/ -w-all
nasm -o i386.o -felf32 peops/i386.asm -I ./peops/ -w-all
#gcc -c -o interface.o peops/interface.c #IGNORING GTK COMPONENT
gcc -c -o key.o peops/key.c -m32 -g
gcc -c -o menu.o peops/menu.c -m32 -g
gcc -c -o prim.o peops/prim.c -m32 -g
#gcc -c -o record.o peops/record.c #IGNORING WINDOWS-ONLY COMPONENT
gcc -c -o soft.o peops/soft.c -m32 -g
#gcc -c -o support.o peops/support.c #IGNORING GTK COMPONENT
gcc -c -o zn.o peops/zn.c -m32 -g

#-------------------------------------------------------------------------#
# LINK                                                                    #
#-------------------------------------------------------------------------#
gcc -o stationary_test *.o $(sdl2-config --libs) -lm -m32  -g #ld -melf_i386