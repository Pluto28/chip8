
all:
	@cd ./src && make
	@cp ./src/chip8 . 
clean:
	@cd ./src && make clean
