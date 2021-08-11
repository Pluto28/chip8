SHELL = /bin/sh
CC = gcc
# linker
linker_flags = $(shell sdl2-config --libs)

# compiler options
cc_options = -O2

# objects
objects = graphics.o chip8.o opcodes.o

chip8: $(objects)
	$(CC) $(cc_options) $(linker_flags) -o chip8

graphics.o: graphics.c graphics.h chip8.h
	$(CC) $(cc_options) -c graphics.c

chip8.o: chip8.c graphics.h chip8.h
	$(CC) $(cc_options) $(linker_flags) -o chip8.c

opcodes.o: opcodes.c chip8.h opcodes.h graphics.h
	$(CC) $(cc_options) $(linker_flags) -o chip8.c

clean: 
	@$(RM) $(objects)