#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

#include "qoi_header.h"
#include "qoi_reader.h"

#include <iostream>
#include <cstdint>

//Globals
uint32_t* gFrameBuffer; //Array of pixels
SDL_Window* gSDLWindow;
SDL_Renderer* gSDLRenderer;
SDL_Texture* gSDLTexture;
Qoi_Header header; //Holds header information about file

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

  SDL_Delay(1); //Sleep this many milliseconds

  return true;
}

bool render(){
  char* pix; //Pointer to locked pixels / our texture
  int pitch; //This is the length of one row of locked pixels

  SDL_LockTexture(gSDLTexture, NULL, (void**)&pix, &pitch); //Lock entire (NULL) texture for write only access
  for(int i = 0, sp = 0, dp = 0; i < header.height; ++i, dp += header.width, sp += pitch){ //Iterate each row
    memcpy(pix + sp, gFrameBuffer + dp, header.width * 4); //Copy each pixel from our framebuffer to our texture
  }

  SDL_UnlockTexture(gSDLTexture);
  SDL_RenderTexture(gSDLRenderer, gSDLTexture, NULL, NULL); //NULLs are for rects being entire texture/target
  SDL_RenderPresent(gSDLRenderer); //Update the screen

  return true;
}

int main(int argc, char** argv){
  if(argc <2){
    std::cerr << "Requires argument\n";
    return -1;
  }

  //Read qoi header
  if(!read_qoi_header(argv[1], &header)){
    return -1;
  }
  
  //Allocate space for our framebuffer
  gFrameBuffer = new uint32_t[header.width * header.height]{0xffffffff};

  //Read our qoi data
  if(!read_qoi(argv[1], gFrameBuffer, &header)){
    std::cerr << "There was an error reading the file so the final image might turn out wrong\n";
  }

  //Initialize SDL
  if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)){
    return -1;
  }

  gSDLWindow = SDL_CreateWindow(argv[1], header.width, header.height, 0);
  gSDLRenderer = SDL_CreateRenderer(gSDLWindow, NULL); //Pointer to window, name of driver (or NULL for auto)
  gSDLTexture = SDL_CreateTexture(
    gSDLRenderer, //Pointer to renderer
    SDL_PIXELFORMAT_RGBA8888, //Format = RGBA with 8 bit values for each
    SDL_TEXTUREACCESS_STREAMING, //Access pattern
    header.width, header.height);

  if(!gFrameBuffer || !gSDLWindow || !gSDLRenderer || !gSDLTexture){ //Something is broken
    return -1;
  }

  if(!render()){
    return -1;
  }

  //Main Loop
  while(update()){
  }

  //Cleanup in reverse order
  SDL_DestroyTexture(gSDLTexture);
  SDL_DestroyRenderer(gSDLRenderer);
  SDL_DestroyWindow(gSDLWindow);
  SDL_Quit();

  return 0;
}
