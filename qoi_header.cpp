#include "qoi_header.h"

#include <fstream>
#include <iostream>

bool read_qoi_header(const char* filename, QoiHeader* header){
  //Open file
  std::ifstream file;
  file.open(filename);
  if(!file.is_open()){
    std::cerr << filename << " - No loadable images specified.\n";
    return false;
  }
  file.seekg(0, std::ios::beg);

  //4 magic bytes should equal qoif
  file.read(header->magic, 4);
  if(header->magic[0] != 'q' || header->magic[1] != 'o' || header->magic[2] != 'i' || header->magic[3] != 'f'){
    std::cerr << filename << " - Does not look like a qoi image (magic bytes missing)\n";
    return false;
  }

  char buffer[sizeof(uint32_t)]; //uint32_t size char buffer to make filereads easier

  //Image width and height in pixels
  file.read(buffer, sizeof(uint32_t));
  header->width = //Shift each byte to appropriate location
    static_cast<uint32_t>(buffer[0])<<24 | 
    static_cast<uint32_t>(buffer[1])<<16 | 
    static_cast<uint32_t>(buffer[2])<<8 | 
    static_cast<uint32_t>(buffer[3]);
  file.read(buffer, sizeof(uint32_t));
  header->height = //Shift each byte to appropriate location
    static_cast<uint32_t>(buffer[0])<<24 | 
    static_cast<uint32_t>(buffer[1])<<16 | 
    static_cast<uint32_t>(buffer[2])<<8 | 
    static_cast<uint32_t>(buffer[3]);
  std::cout << "Image width: " << header->width << '\n';
  std::cout << "Image height: " << header->height << '\n';

  //3 = RGB, 4 = RGBA
  file.read(reinterpret_cast<char*>(&header->channels), sizeof(uint8_t));
  std::cout << "Channels: ";
  switch(header->channels){
    case 3: std::cout << "RGB"; break;
    case 4: std::cout << "RGBA"; break;
    default: std::cout << "Invalid channels in header"; return false;
  }
  std::cout << '\n';

  //0 = sRGB with linear alpha, 1 = all channels linear
  file.read(reinterpret_cast<char*>(&header->colorspace), sizeof(uint8_t));
  std::cout << "Colorspace: ";
  switch(header->colorspace){
    case 0: std::cout << "sRGB with linear alpha"; break;
    case 1: std::cout << "all channels linear"; break;
    default: std::cout << "Invalid colorspace in header"; return false;
  }
  std::cout << '\n';

  file.close();

  return true;
}
