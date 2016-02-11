#!/bin/sh
#-------------------------------------------------------------------------#
# STATIONARY COMPONENTS                                                   #
#-------------------------------------------------------------------------#
gcc -c -o stationary_test_main.o stationary_test_main.c

#-------------------------------------------------------------------------#
# PEOPS COMPONENTS                                                        #
#-------------------------------------------------------------------------#
gcc -c `pkg-config --cflags gtk+-3.0` -o callbacks.o  peops/callbacks.c `pkg-config --libs gtk+-3.0`
gcc -c -o cfg.o peops/cfg.c
#gcc -c -o conf.o peops/conf.c #IGNORING GTK COMPONENT
gcc -c -o draw.o peops/draw.c 
gcc -c -o DrawString.o peops/DrawString.c 
gcc -c -o fps.o peops/fps.c 
gcc -c -o fpsewp.o  peops/fpsewp.c -I ./peops/fpse
gcc -c -o gpu.o peops/gpu.c
nasm -o hq2x16.o -felf peops/hq2x16.asm -I ./peops/
nasm -o hq2x32.o -felf peops/hq2x32.asm -I ./peops/
nasm -o hq3x16.o -felf peops/hq3x16.asm -I ./peops/
nasm -o hq3x32.o -felf peops/hq3x32.asm -I ./peops/
nasm -o i386.o -felf peops/i386.asm -I ./peops/
#gcc -c -o interface.o peops/interface.c #IGNORING GTK COMPONENT
gcc -c -o key.o peops/key.c
gcc -c -o menu.o peops/menu.c
gcc -c -o prim.o peops/prim.c
#gcc -c -o record.o peops/record.c #IGNORING WINDOWS-ONLY COMPONENT
gcc -c -o soft.o peops/soft.c
#gcc -c -o support.o peops/support.c #IGNORING GTK COMPONENT
gcc -c -o zn.o peops/zn.c

#-------------------------------------------------------------------------#
# LINK                                                                    #
#-------------------------------------------------------------------------#
gcc -o stationary_test *.o