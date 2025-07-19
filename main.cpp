#include <iostream>
#include "qoi_header.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include <fstream>

//Global constants
uint32_t* gFrameBuffer; //Array of pixels
SDL_Window* gSDLWindow;
SDL_Renderer* gSDLRenderer;
SDL_Texture* gSDLTexture;
static int gDone; //Flag for terminating the program (optional)
qoi_header header;

//Update our texture and render the new frame
bool update(){
  SDL_Event e; //Container for events (close window, keypress, etc)
  if(SDL_PollEvent(&e)){ //Poll for new events
    if(e.type == SDL_EVENT_QUIT){ //Window close button pressed
      return false;
    }
    if(e.type == SDL_EVENT_KEY_DOWN && e.key.key == SDLK_ESCAPE){ //Escape key pressed
      return false;
    }
  }

  char* pix; //Pointer to locked pixels / our texture
  int pitch; //This is the length of one row of locked pixels

  SDL_LockTexture(gSDLTexture, NULL, (void**)&pix, &pitch); //Lock entire (NULL) texture for write only access
  for(int i = 0, sp = 0, dp = 0; i < header.height; ++i, dp += header.width, sp += pitch){ //Iterate each row
    memcpy(pix + sp, gFrameBuffer + dp, header.width * 4); //Copy each pixel from our framebuffer to our texture
  }

  SDL_UnlockTexture(gSDLTexture);
  SDL_RenderTexture(gSDLRenderer, gSDLTexture, NULL, NULL); //NULLs are for rects being entire texture/target
  SDL_RenderPresent(gSDLRenderer); //Update the screen
  //SDL_Delay(1); //Sleep this many milliseconds

  return true;
}

//Main Loop
void loop(){
  if(!update()){ //If we're done we set gDone to 1
    gDone = 1;
  }
}

bool readQiv(const char* filename){
  std::ifstream file;
  file.open(filename);
  if(!file.is_open()){
    return false;
  }

  //get filesize TODO: Maybe remove
  file.seekg(0, std::ios::end);
  auto end = file.tellg();

  //Seek to start of file
  file.seekg(0, std::ios::beg);
  std::cout << "pos: " << file.tellg() << '\n';

  std::cout << "size: " << sizeof(header) << '\n';
  //Qoi header is 14 bytes long
  //4 magic bytes qoif
  file.read(header.magic, 4);
  std::cout << header.magic[0] << header.magic[1] << header.magic[2] << header.magic[3] << '\n';
  if(header.magic[0] != 'q' || header.magic[1] != 'o' || header.magic[2] != 'i' || header.magic[3] != 'f'){
    std::cerr << filename << " - Does not look like a qoi image (magic bytes missing)\n";
    return false;
  }

  char buffer[sizeof(uint32_t)]; //uint32_t size char buffer to make filereads easier

  //Image width and height in pixels
  file.read(buffer, sizeof(uint32_t));
  header.width = (uint32_t)buffer[0]<<24 | (uint32_t)buffer[1]<<16 | (uint32_t)buffer[2]<<8 | (uint32_t)buffer[3];
  file.read(buffer, sizeof(uint32_t));
  header.height = (uint32_t)buffer[0]<<24 | (uint32_t)buffer[1]<<16 | (uint32_t)buffer[2]<<8 | (uint32_t)buffer[3];
  std::cout << "Image width: " << header.width << '\n';
  std::cout << "Image height: " << header.height << '\n';

  //3 = RGB, 4 = RGBA
  file.read(reinterpret_cast<char*>(&header.channels), sizeof(uint8_t));
  std::cout << "Channels: ";
  switch(header.channels){
    case 3: std::cout << "RGB"; break;
    case 4: std::cout << "RGBA"; break;
    default: std::cout << "Invalid channels in header"; return false;
  }
  std::cout << '\n';

  //0 = sRGB with linear alpha, 1 = all channels linear
  file.read(reinterpret_cast<char*>(&header.colorspace), sizeof(uint8_t));
  std::cout << "Colorspace: ";
  switch(header.colorspace){
    case 0: std::cout << "sRGB with linear alpha"; break;
    case 1: std::cout << "all channels linear"; break;
    default: std::cout << "Invalid colorspace in header"; return false;
  }
  std::cout << '\n';

  std::cout << "pos: " << file.tellg() << '\n';


  file.close();

  return true;
}

int main(int argc, char** argv){
  //Read file
  if(!readQiv(argv[1])){
    return -1;
  }

  //Initialize SDL
  if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)){
    return -1;
  }

  gFrameBuffer = new uint32_t[header.width * header.height];
  gSDLWindow = SDL_CreateWindow("SDL3 window", header.width, header.height, 0);
  gSDLRenderer = SDL_CreateRenderer(gSDLWindow, NULL); //Pointer to window, name of driver (or NULL for auto)
  gSDLTexture = SDL_CreateTexture(
    gSDLRenderer, //Pointer to renderer
    SDL_PIXELFORMAT_ARGB8888, //Format = ARGB with 8 bit values for each
    SDL_TEXTUREACCESS_STREAMING, //Access pattern
    header.width, header.height);

  if(!gFrameBuffer || !gSDLWindow || !gSDLRenderer || !gSDLTexture){ //Something is broken
    return -1;
  }

  gDone = 0;

  //Main Loop
  while(!gDone){
    loop();
  }

  //Cleanup in reverse order
  SDL_DestroyTexture(gSDLTexture);
  SDL_DestroyRenderer(gSDLRenderer);
  SDL_DestroyWindow(gSDLWindow);
  SDL_Quit();

  return 0;
}
