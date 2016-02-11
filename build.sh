gcc -c `pkg-config --cflags gtk+-3.0` -o callbacks.o  peops/callbacks.c `pkg-config --libs gtk+-3.0`
gcc -c -o cfg.o peops/cfg.c
gcc -c -o draw.o peops/draw.c 
gcc -c -o DrawString.o peops/DrawString.c 
gcc -c -o fps.o peops/fps.c 
gcc -c -o fpsewp.o  peops/fpsewp.c -I ./peops/fpse