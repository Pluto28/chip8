SHELL = /bin/sh
CC = gcc

# linker
linker_flags = $(shell sdl2-config --libs)

# compiler options
cc_options = -O0

cc_options += $(shell sdl2-config --cflags)

# objects
objects = graphics.o chip8.o opcodes.o

chip8: $(objects)
	$(CC) -o chip8 $(objects) $(cc_options) $(linker_flags) 

graphics.o: graphics.c graphics.h chip8.h
	$(CC) $(cc_options) -c graphics.c

chip8.o: chip8.c graphics.h chip8.h
	$(CC) $(cc_options) -c chip8.c

opcodes.o: opcodes.c chip8.h opcodes.h graphics.h
	$(CC) $(cc_options) -c opcodes.c

clean: 
	$(RM) $(objects)
