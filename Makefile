# CPATH specifies which folders contain the header files for the project
export CPATH = include

# COMPILER_FLAGS are the additional compiler options we're using
COMPILER_FLAGS = 

# CC is the compiler to use
CC = gcc

# LINKER_FLAGS are the libraries we are linking against
LINKER_FLAGS = -lSDL2

# OBJS specify the name of which files to compile as part of the project
OBJS = funcs.s graphics.c chip8.c

# OBJS_DIR is where the compiled object files will be stored
OBJS_DIR = src

# OBJ_NAME is used to specify the name of the the final executable
OBJ_NAME = chip8

all : objs
	$(CC) $(OBJS_DIR)/*.o -o $(OBJ_NAME)

objs: $(OBJS)
	$(CC) -c $(OBJS) $(LINKER_FLAGS)
	mv *.o $(OBJS_DIR)/

clean: 
	rm $(OBJS_DIR)/*.o
