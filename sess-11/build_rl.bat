PATH=C:\raylib\w64devkit\bin;%PATH%
gcc -o tool.exe main.c wav.c fft.c util.c bmp.c gauss.c lpc.c formant.c root.c  C:\raylib\raylib\src\raylib.rc.data -s -static -O2 -std=c99 -Wall -IC:\raylib\raylib\src -Iexternal -DPLATFORM_DESKTOP -lraylib -lopengl32 -lgdi32 -lwinmm -lm
.\tool.exe
pause

