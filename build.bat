
rem -------------------------------------------------------------------------#
rem  STATIONARY COMPONENTS                                                   #
rem -------------------------------------------------------------------------#
cl /c /out:stationary_test_main.obj stationary_test_main.c

rem -------------------------------------------------------------------------#
rem  PEOPS COMPONENTS                                                        #
rem -------------------------------------------------------------------------#
cl /c /out:conf.obj peops\conf.c /D_WINDOWS
cl /c /out:cfg.obj peops\cfg.c /D_WINDOWS
cl /c /out:draw.obj peops\draw.c /D_WINDOWS
cl /c /out:DrawString.obj peops\DrawString.c /D_WINDOWS 
cl /c /out:fps.obj peops\fps.c /D_WINDOWS
cl /c /out:gpu.obj peops\gpu.c /D_WINDOWS
nasm /out:hq2x16.obj -fwin32 peops\hq2x16.asm -I .\peops\ -w-all
nasm /out:hq2x32.obj -fwin32 peops\hq2x32.asm -I .\peops\ -w-all
nasm /out:hq3x16.obj -fwin32 peops\hq3x16.asm -I .\peops\ -w-all
nasm /out:hq3x32.obj -fwin32 peops\hq3x32.asm -I .\peops\ -w-all
nasm /out:i386.obj -fwin32 peops\i386.asm -I .\peops\ -w-all
cl /c /out:key.obj peops\key.c /D_WINDOWS
cl /c /out:menu.obj peops\menu.c /D_WINDOWS
cl /c /out:prim.obj peops\prim.c /D_WINDOWS
cl /c /out:record.obj peops\record.c /D_WINDOWS
cl /c /out:soft.obj peops\soft.c /D_WINDOWS
cl /c /out:zn.obj peops\zn.c /D_WINDOWS

rem -------------------------------------------------------------------------#
rem  LINK                                                                    #
rem -------------------------------------------------------------------------#
link32 /out:stationary_test.exe *.obj ddraw.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
