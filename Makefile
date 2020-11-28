# CPATH specifies which folders contain the header files for the project
export CPATH = include

# Path to look for source code
VPATH=./src

# COMPILER_FLAGS are the additional compiler options we're using
COMPILER_FLAGS = -DDEBUG 

# CC is the compiler to use
CC = gcc

# LINKER_FLAGS are the libraries we are linking against
LINKER_FLAGS = -lSDL2

# OBJS_DIR is where the compiled object files will be stored
OBJS_DIR = src

# OBJ_NAME is used to specify the name of the the final executable
OBJ_NAME = chip8

all : chip8.o graphics.o
	$(CC) $(OBJS_DIR)/*.o -o $(OBJ_NAME) $(COMPILER_FLAGS) $(LINKER_FLAGS)

chip8.o: 
	$(CC) -c chip8.c $(COMPILER_FLAGS) $(LINKER_FLAGS)
	mv chip8.o $(OBJS_DIR)

graphics.o: 
	$(CC) -c graphics.c $(COMPILER_FLAGS) $(LINKER_FLAGS)
	mv graphics.o $(OBJS_DIR)

clean: 
	rm $(OBJS_DIR)/*.o
