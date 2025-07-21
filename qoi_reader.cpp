#include <iostream> //TODO: This is just for testing
#include <bitset>
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

#include "qoi_header.h"
#include "qoi_reader.h"
#include <cstdint>
#include <fstream>

bool read_qoi(const char* filename, uint32_t* frame_buffer, Qoi_Header* header){
  uint32_t prev_px{0x00'00'00'ff}; //Decoder starts with {r: 0, g: 0, b: 0, a: 255} as the previous pixel
  uint32_t array[64]{0}; //Running array of previously seen pixel values
  uint8_t tag; //Holds tag for current "chunk"
  QOI_OP op;
  int index_position;
  int frame_buffer_index{0};

  std::ifstream file(filename);
  if(!file.is_open()){
    return false;
  }
  file.seekg(14, std::ios::beg); //Seek after header //TODO: PUT ME BACK

  //Read first byte
  std::cout << file.tellg() << ": ";
  file.read(reinterpret_cast<char*>(&tag), 1);
  std::cout << std::bitset<8>(tag) << " == ";

  op = read_tag(tag);

  for(int i{0}; i<335; ++i){
    //SDL_Delay(20);
    std::cout << "index " << file.tellg() << ": ";
    file.read(reinterpret_cast<char*>(&tag), 1);
    std::cout << std::bitset<8>(tag) << " == ";

    op = read_tag(tag);
    switch(op){
      case QOI_OP::RGB: break;
      case QOI_OP::RGBA:
        file.read(reinterpret_cast<char*>(&prev_px), 4); //TODO: THIS iS PROBABLY WRONG
        frame_buffer[frame_buffer_index] = prev_px;
        index_position = ( //Hash function for the index //TODO: Make this an inline function or something
          (frame_buffer[frame_buffer_index] & 0xff00'0000 /*r*/)*3 +
          (frame_buffer[frame_buffer_index] & 0x00ff'0000 /*g*/)*5 +
          (frame_buffer[frame_buffer_index] & 0x0000'ff00 /*b*/)*7 +
          (frame_buffer[frame_buffer_index] & 0x0000'00ff /*a*/)*11
        ) % 64;
        array[index_position] = frame_buffer[frame_buffer_index]; //Copy our previously seen pixel into our array at the index_position calculated by the hash function
        frame_buffer_index++;
        break;
      case QOI_OP::INDEX: 
        frame_buffer[frame_buffer_index] = array[tag & 0b0011'1111]; //Copy value by 6-bit index into the array to our framebuffer
        prev_px = frame_buffer[frame_buffer_index]; //Copy most recent pixel
        frame_buffer_index++;
        break;
      case QOI_OP::DIFF: break;
      case QOI_OP::LUMA:
        uint8_t byte;
        int8_t dr, dg, db;
        int8_t cur_r, cur_g, cur_b;
        int8_t dr_dg, db_dg;

        dg = (tag & 0b0001'1111)*( (tag & 0b0010'0000) ? (1) : (-1)); //Diff green
        std::cout << "dg: " << dg << '\n';
        std::cout << "dg: " << (tag & 0b0001'1111)*( (tag & 0b0010'0000) ? (1) : (-1)) << '\n';
        cur_g = ( ( (prev_px & 0x00ff'0000)>>16 ) - dg ) % 256; //Current green
        std::cout << "prev_g: " << ((prev_px & 0x00ff'0000)>>16) << '\n'; //Prev green
        std::cout << "cur_g: " << cur_g << '\n';
        std::cout << "cur_g: " << ( ( (prev_px & 0x00ff'0000)>>16 ) - dg ) % 256 << '\n';

        file.read(reinterpret_cast<char*>(&byte), 1);
        dr_dg = ((byte>>4) & 0b0000'0111)*( (byte & 0b1000'0000) ? (1) : (-1)); //dr - dg
        std::cout << "dr_dg: " << dr_dg << '\n';
        std::cout << "dr_dg: " << ((byte>>4) & 0b0000'0111)*( (byte & 0b1000'0000) ? (1) : (-1)) << '\n';
        dr = dr_dg + dg; //Diff red
        std::cout << "dr: " << dr << '\n';
        std::cout << "dr: " << dr_dg + dg << '\n';
        cur_r = ( ((prev_px & 0xff00'0000)>>24) + dr) % 256; //Current red
        std::cout << "cur_r: " << cur_r << '\n';
        std::cout << "cur_r: " << ( ((prev_px & 0xff00'0000)>>24) + dr) % 256 << '\n';

        //cur_r = (dr - (prev_px & 0xff00'0000)>>24) - (dg - (prev_px & 0x00ff'0000)>>16) % 256); //Current red

        //std::cout << "b: " << (byte & 0b0000'0111)*( (byte & 0b0000'1000) ? (1) : (-1)) << '\n';
        //db = (byte & 0b0000'0111)*( (byte & 0b0000'1000) ? (1) : (-1));
        //std::cout << "dr_dg: " << ( (dr - (prev_px & 0xff00'0000)>>24) - (dg - (prev_px & 0x00ff'0000)>>16) % 256) << '\n';
        //std::cout << "db_dg: " << ( (db - (prev_px & 0x0000'ff00)>>24) - (dg - (prev_px & 0x00ff'0000)>>16) % 256) << '\n';
        //db_dg = ( (db - (prev_px & 0x0000'ff00)>>24) - (dg - (prev_px & 0x00ff'0000)>>16) % 256);

        frame_buffer[frame_buffer_index] = (
          cur_r<<24 |
          cur_g<<16 |
          cur_b<<8 |
          (prev_px & 0x0000'00ff)
        );

        prev_px = frame_buffer[frame_buffer_index]; //Copy the previous pixel
        index_position = ( //Hash function for the index //TODO: Make this an inline function or something
          (frame_buffer[frame_buffer_index] & 0xff00'0000 /*r*/)*3 +
          (frame_buffer[frame_buffer_index] & 0x00ff'0000 /*g*/)*5 +
          (frame_buffer[frame_buffer_index] & 0x0000'ff00 /*b*/)*7 +
          (frame_buffer[frame_buffer_index] & 0x0000'00ff /*a*/)*11
        ) % 64;
        array[index_position] = frame_buffer[frame_buffer_index]; //Copy our previously seen pixel into our array at the index_position calculated by the hash function

        break;
      case QOI_OP::RUN:
        //We don't need to copy our pixel into our most recent pixel variable or fill in our previously seen pixels array since their values would be overwritten by themselves
        for(int run{0}; run<(tag & 0b0011'1111)+1; ++run){ //6-bit run-length repeating of previous pixel
          //TODO: This looks like some memcpy tomfoolery
          frame_buffer[frame_buffer_index] = prev_px;
          frame_buffer_index++;
        }
        break;
    }
  }

  //TODO: Remove
  //for(int i{0}; i<8192; ++i, ++frame_buffer_index){
  //  frame_buffer[frame_buffer_index] = 0xffffffff;
  //}

  file.close();
  return true;
}

QOI_OP read_tag(const uint8_t tag){
  //Check for QOI_OP_RGB and QOI_OP_RGBA tags
  if(tag == 0b1111'1110){
    std::cout << "RGB\n";
    return QOI_OP::RGB;
  }
  if(tag == 0b1111'1111){
    std::cout << "RGBA\n";
    return QOI_OP::RGBA;
  }

  //Check for two bit tags
  if((tag>>6) == 0b0000'0011){
    std::cout << "RUN\n";
    return QOI_OP::RUN;
  }
  if((tag>>6) == 0b0000'0010){
    std::cout << "LUMA\n";
    return QOI_OP::LUMA;
  }
  if((tag>>6) == 0b0000'0001){
    std::cout << "DIFF\n";
    return QOI_OP::DIFF;
  }
  if((tag>>6) == 0b0000'0000){
    std::cout << "INDEX\n";
    return QOI_OP::INDEX;
  }

  return QOI_OP::MAX_TAG;
}
