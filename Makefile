CC = gcc          # compiler command
CFLAGS = -O2      # compiler flags
LDFLAGS = -lSDL2  # linker flags 


all: opcode.o chip8.o graphics.o
    $(CC) -o chip8 opcode.o chip8

opcode.o: opcode.c opcode.h

chip8.o: chip8.c chip8.h

graphics.o: graphics.c graphics.h
    $(CC) $(CFLAGS) -c graphics.c

clean: 
	@rm -f *.o
