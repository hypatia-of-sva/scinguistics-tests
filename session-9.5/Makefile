CC = gcc
CFLAGS = -Wall -Werror
#-std=c90
LDFLAGS =  -lm -L../tool_2/raylib/src/ -lraylib
#-lgsl -lgslcblas
IFLAGS = -I. -I../tool_2/raylib/src/

SOURCES = main.c wav.c fft.c util.c bmp.c gauss.c lpc.c formant.c root.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = wav-tool

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@  $^ $(IFLAGS)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
