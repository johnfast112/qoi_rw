# qoi_rw
A commandline tool for reading and writing and displaying QOI files

## Compiling
For compiling on unix-like systems:
```
mkdir build
cd build
cmake ..
make
```
An executable named qoi_rw will appear and can be used alongside a .qoi image to read and display it 

For compiling on windows:
I recommend installing [MSYS2](https://www.msys2.org/)
After installing MSYS2 run `pacman -Syu` until you recieve the "there is nothing to do" message. This is usually twice for me.
Run the following command to install all the necessary build tools:
```
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-sdl3
```
Build the program with:
```
mkdir build
cmake -S . -B build
cmake --build build
```
