#include "qoi_header.h"
#include <fstream>
#include <iostream>

bool read_qoi_header(const char* filename, Qoi_Header* header){
  //Open file
  std::ifstream file;
  file.open(filename, std::ios::binary);
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

  char buffer[4];
  //Image width and height in pixels
  file.read(buffer, 4); //Width
  header->width = (
    ((buffer[0]<<24) & 0xff00'0000) |
    ((buffer[1]<<16) & 0x00ff'0000) |
    ((buffer[2]<<8) & 0x0000'ff00) |
    ((buffer[3]) & 0x0000'00ff)
  );
  file.read(buffer, 4); //Height
  header->height = (
    ((buffer[0]<<24) & 0xff00'0000) |
    ((buffer[1]<<16) & 0x00ff'0000) |
    ((buffer[2]<<8) & 0x0000'ff00) |
    ((buffer[3]) & 0x0000'00ff)
  );

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
