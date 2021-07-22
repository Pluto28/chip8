# chip8

This is a simple chip8 emulator made with c and sdl2, i really enjoyed the project
and i hope you can use it for something usefull

### Install SDL2

To compile this project you need to have the SDL2 library installed. Follow the 
instructions to install SDL2 for your respective platform

- Ubunutu/Debian:\
    `sudo apt-get install libsdl2-dev`

- Fedora:\
    `sudo yum install SDL2-devel`

- Gentoo:\
    `sudo emerge libsdl2`

- Arch Linux:\
    `sudo pacman -S sdl2`

If your platform is not listed here, please refer to the [official documentation](https://wiki.libsdl.org/Installation)

### Compilation

Run `make` inside the chip8 directory

### TODO
    - [ ] Text debug probe with pausing habilities

    - [ ] Add support for posix compliant command lines options using argv

    - [ ] Rewrite the graphics engine to be more efficient
