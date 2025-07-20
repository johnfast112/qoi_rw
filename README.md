# qoi_rw
A commandline tool for reading and writing and displaying QOI files

## Cloning
This repo includes SDL as a submodule so make sure to include it when cloning by adding `--recursive` to the end of your command
```
git clone https://github.com/johnfast112/qoi_rw.git --recursive
cd qoi_rw
```

## Building
### For compiling on unix-like systems:
```
mkdir build
cd build
cmake ..
make
```
An executable named qoi_rw will appear and can be used alongside a .qoi image to read and display it 

### For compiling on windows:
I recommend installing [MSYS2](https://www.msys2.org/)  
After installing MSYS2 run `pacman -Syu` until you recieve the "there is nothing to do" message. This is usually twice for me.  
  
Run the following command to install all the necessary build tools:
```
pacman -S mingw-w64-ucrt-x86_64-gcc \
mingw-w64-ucrt-x86_64-ninja \
mingw-w64-ucrt-x86_64-cmake \
mingw-w64-ucrt-x86_64-sdl3 \
git \
cmake
```
Build the program with:
```
mkdir build
cmake -S . -B build
cmake --build build
```
To be able to use the program outside of msys2, the following .dll files must be present in the folder: `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, `libwinpthread-1.dll`, `SDL3.dll`  
  
The first three should already be on your system from installing msys2, and the following command should copy them to your build directory:
```
cp /ucrt64/bin/{libgcc_s_seh-1.dll,libstdc++-6.dll,libwinpthread-1.dll} ./build
```
If this fails you can manually search for them with
```
find / -name "libgcc_s_seh-1.dll" -o -name "libstdc++-6.dll" -o -name "libwinpthread-1.dll"
```

The last one will be available under the [releases](https://github.com/libsdl-org/SDL/releases) page in the SDL git repo. Find and download the newest release with the name most similar to `SDL3-devel-X.X.XX-mingw.tar.gz` (the .zip also works) and decompress it. Inside the `SDL3-devel-X.X.XX-mingw` directory you will find a `i686-w64-mingw32` directory and a `x86_64-w64-mingw32` directory. If you are on a 32-bit system navigate to the `i686` directory, otherwise use the `x86_64` one. Copy the `SDL3.dll` file from the bin folder to your original build directory. The following script simply automates this process:
```
sh download_SDL3_dll.sh
```
  
The `qoi_rw.exe` executable should now be accessable by applications outside of msys2, such as cmd.exe or windows shortcuts
