#!/bin/sh
#-------------------------------------------------------------------------#
# STATIONARY COMPONENTS                                                   #
#-------------------------------------------------------------------------#
gcc -c -o stationary_test_main.o stationary_test_main.c #-m32

#-------------------------------------------------------------------------#
# PEOPS COMPONENTS                                                        #
#-------------------------------------------------------------------------#
gcc -c `pkg-config --cflags gtk+-3.0` -o callbacks.o  peops/callbacks.c `pkg-config --libs gtk+-3.0` #-m32 
gcc -c -o cfg.o peops/cfg.c #-m32
#gcc -c -o conf.o peops/conf.c #IGNORING GTK COMPONENT
gcc -c -o draw.o peops/draw.c #-m32
gcc -c -o DrawString.o peops/DrawString.c #-m32 
gcc -c -o fps.o peops/fps.c #-m32
gcc -c -o fpsewp.o  peops/fpsewp.c -I ./peops/fpse #-m32
gcc -c -o gpu.o peops/gpu.c #-m32
nasm -o hq2x16.o -felf64 peops/hq2x16.asm -I ./peops/ -W-warning
nasm -o hq2x32.o -felf64 peops/hq2x32.asm -I ./peops/ -W-warning
nasm -o hq3x16.o -felf64 peops/hq3x16.asm -I ./peops/ -W-warning
nasm -o hq3x32.o -felf64 peops/hq3x32.asm -I ./peops/ -W-warning
nasm -o i386.o -felf64 peops/i386.asm -I ./peops/ -W-warning
#gcc -c -o interface.o peops/interface.c #IGNORING GTK COMPONENT
gcc -c -o key.o peops/key.c #-m32
gcc -c -o menu.o peops/menu.c #-m32
gcc -c -o prim.o peops/prim.c #-m32
#gcc -c -o record.o peops/record.c #IGNORING WINDOWS-ONLY COMPONENT
gcc -c -o soft.o peops/soft.c #-m32
#gcc -c -o support.o peops/support.c #IGNORING GTK COMPONENT
gcc -c -o zn.o peops/zn.c #-m32

#-------------------------------------------------------------------------#
# LINK                                                                    #
#-------------------------------------------------------------------------#
gcc -o stationary_test *.o $(pkg-config --libs x11) #-m32 #ld -melf_i386