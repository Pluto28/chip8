SHELL = /bin/sh
CC = gcc

# linker
linker_flags = $(shell sdl2-config --libs)

# compiler options
cc_options = 

cc_options += $(shell sdl2-config --cflags)

# objects
objects = graphics.o chip8.o opcodes.o

chip8: $(objects)
	$(CC) -o chip8 $(objects) $(cc_options) $(linker_flags) 

graphics.o: graphics.c graphics.h chip8.h
	$(CC) -c graphics.c $(cc_options)

chip8.o: chip8.c graphics.h chip8.h
	$(CC) -c chip8.c $(cc_options)

opcodes.o: opcodes.c chip8.h opcodes.h graphics.h
	$(CC) -c opcodes.c $(cc_options) 

clean: 
	$(RM) $(objects)
