#include <iostream> //TODO: This is just for testing
#include <bitset>

#include "qoi_header.h" //TODO: Maybe unused
#include "qoi_reader.h"
#include <cstdint>
#include <fstream>

bool read_qoi(const char* filename, uint32_t* frame_buffer, Qoi_Header* header){
  uint32_t prev_px{0x00'00'00'ff}; //Decoder starts with {r: 0, g: 0, b: 0, a: 255} as the previous pixel
  char px_buf[4]; //Array for buffer of rgba
  uint32_t array[64]{0}; //Running array of previously seen pixel values
  uint8_t tag; //Holds tag for current "chunk"
  QOI_OP op;
  int frame_buffer_index{0};

  std::ifstream file(filename);
  if(!file.is_open()){
    return false;
  }
  file.seekg(14, std::ios::beg); //Seek after header

  while(frame_buffer_index<header->width*header->height){
  //for(int i{0}; i<294; ++i){
    std::cout << "index " << file.tellg();
    std::cout << " (pixel " << frame_buffer_index << "): ";
    file.read(reinterpret_cast<char*>(&tag), 1);
    std::cout << std::bitset<8>(tag) << " == ";

    op = read_tag(tag);
    switch(op){
      case QOI_OP::RGB: //Read the following 3 bytes as RGB, alpha remains unchanged
        file.read(px_buf, 3); //Read
        px_buf[3] = prev_px & 0x0000'00ff; //Copy alpha value so it remains unchanged
        prev_px = arr_to_px(px_buf); //Store as last pixel
        frame_buffer[frame_buffer_index] = prev_px; //Write to frame_buffer
        array[calc_index_hash(prev_px)] = prev_px; //Copy pixel to array at hashed index
        frame_buffer_index++; //Increment pixel index
        std::cout << " r: " << ((prev_px>>24) & 0x0000'00ff) << " g: " << ((prev_px>>16) & 0x0000'00ff) << " b: " << ((prev_px>>8) & 0x0000'00ff) << " a: " << (prev_px & 0x0000'00ff) << " hash: " << calc_index_hash(prev_px); //Debug info
        break;
      case QOI_OP::RGBA: //Read the following 4 bytes as RGBA
        file.read(px_buf, 4); //Read
        prev_px = arr_to_px(px_buf); //Store as last pixel
        frame_buffer[frame_buffer_index] = prev_px; //Write to frame_buffer
        array[calc_index_hash(prev_px)] = prev_px; //Copy pixel to array at hashed index
        frame_buffer_index++; //Increment pixel index
        std::cout << " r: " << ((prev_px>>24) & 0x0000'00ff) << " g: " << ((prev_px>>16) & 0x0000'00ff) << " b: " << ((prev_px>>8) & 0x0000'00ff) << " a: " << (prev_px & 0x0000'00ff) << " hash: " << calc_index_hash(prev_px); //Debug info
        break;
      case QOI_OP::INDEX: 
        prev_px = array[tag & 0b0011'1111]; //Read from array and store as last pixel
        frame_buffer[frame_buffer_index] = prev_px; //Write to frame_buffer
        //Don't calculate hash because it will overwrite itself with itself
        frame_buffer_index++; //Increment pixel index
        std::cout << ' ' << (tag & 0b0011'1111) << " r: " << ((prev_px>>24) & 0x0000'00ff) << " g: " << ((prev_px>>16) & 0x0000'00ff) << " b: " << ((prev_px>>8) & 0x0000'00ff) << " a: " << (prev_px & 0x0000'00ff); //Debug info
        break;
      case QOI_OP::DIFF: 
        //TODO: EXPLAIN MEEEEE
        prev_px = ( //Store as last pixel
          ( ( ( ((prev_px & 0xff00'0000)>>24) + ( ((tag & 0b0011'0000)>>4)-2) ) % 256 )<<24)| //Red
          ( ( ( ((prev_px & 0x00ff'0000)>>16) + ( ((tag & 0b0000'1100)>>2)-2) ) % 256 )<<16)| //Green
          ( ( ( ((prev_px & 0x0000'ff00)>>8 ) + ( ((tag & 0b0000'0011)   )-2) ) % 256 )<<8 )| //Blue
          (prev_px & 0x0000'00ff)
        );
        frame_buffer[frame_buffer_index] = prev_px; //Write to frame_buffer
        array[calc_index_hash(prev_px)] = prev_px; //Copy pixel to array at hashed index
        frame_buffer_index++; //Increment pixel index
        std::cout << ' ' << (tag & 0b0011'1111) << " r: " << ((prev_px>>24) & 0x0000'00ff) << " g: " << ((prev_px>>16) & 0x0000'00ff) << " b: " << ((prev_px>>8) & 0x0000'00ff) << " a: " << (prev_px & 0x0000'00ff); //Debug info
        break;
      case QOI_OP::LUMA:
        //TODO: Truncate me
        uint8_t byte; //Unsigned buffer and colors
        uint8_t cur_r, cur_g, cur_b;
        int8_t dr, dg, db; //Signed differences to add negatives
        int8_t dr_dg, db_dg;

        //Green
        dg = (tag & 0b0011'1111)-32; //Diff green
        //std::cout << "dg: " << dg << '\n';
        //std::cout << "dg: " << (tag & 0b0011'1111)-32 << '\n';
        cur_g = ( ( (prev_px & 0x00ff'0000)>>16 ) + dg ) % 256; //Current green
        //std::cout << "prev_g: " << ((prev_px & 0x00ff'0000)>>16) << '\n'; //Prev green
        //std::cout << "cur_g: " << cur_g << '\n';
        //std::cout << "cur_g: " << ( ( (prev_px & 0x00ff'0000)>>16 ) + dg ) % 256 << '\n';

        file.read(reinterpret_cast<char*>(&byte), 1);
        //Red
        dr_dg = ((byte>>4) & 0b0000'1111)-8; //dr - dg
        //std::cout << "dr_dg: " << dr_dg << '\n';
        //std::cout << "dr_dg: " << ((byte>>4) & 0b0000'1111)-8 << '\n';
        dr = dr_dg + dg; //Diff red
        //std::cout << "dr: " << dr << '\n';
        //std::cout << "dr: " << dr_dg + dg << '\n';
        cur_r = ( ((prev_px & 0xff00'0000)>>24) + dr) % 256; //Current red
        //std::cout << "cur_r: " << cur_r << '\n';
        //std::cout << "cur_r: " << ( ((prev_px & 0xff00'0000)>>24) + dr) % 256 << '\n';

        //Blue
        db_dg = (byte & 0b0000'1111)-8; //db - dg
        //std::cout << "db_dg: " << db_dg << '\n';
        //std::cout << "db_dg: " << (byte & 0b0000'1111)-8 << '\n';
        db = db_dg + dg; //Diff blue
        //std::cout << "db: " << db << '\n';
        //std::cout << "db: " << db_dg + dg << '\n';
        cur_b = ( ((prev_px & 0x0000'ff00)>>8) + db) % 256; //Current blue
        //std::cout << "cur_b: " << cur_b << '\n';
        //std::cout << "cur_b: " << ( ((prev_px & 0x0000'ff00)>>8) + db) % 256 << '\n';

        prev_px = ( //Store as last pixel
          cur_r<<24 |
          cur_g<<16 |
          cur_b<<8 |
          (prev_px & 0x0000'00ff)
        );

        frame_buffer[frame_buffer_index] = prev_px; //Copy the previous pixel
        array[calc_index_hash(prev_px)] = prev_px; //Copy pixel to array at hashed index
        frame_buffer_index++; //Increment pixel index
        std::cout << " r: " << ((prev_px>>24) & 0x0000'00ff) << " g: " << ((prev_px>>16) & 0x0000'00ff) << " b: " << ((prev_px>>8) & 0x0000'00ff) << " a: " << (prev_px & 0x0000'00ff) << " hash: " << calc_index_hash(prev_px); //Debug info
        break;
      case QOI_OP::RUN: //Repeat last pixel run-length times
        for(int run{0}; run<(tag & 0b0011'1111)+1; ++run){ //6-bit run-length with bias of -1
          //TODO: This looks like some memcpy tomfoolery
          frame_buffer[frame_buffer_index] = prev_px; //Write pixel
          frame_buffer_index++; //Increment pixel index
        }
        std::cout << ' ' << (tag & 0b0011'1111)+1; //Debug info
        break;
    }

    std::cout << '\n';
  }

  //TODO: Remove
  //for(int i{0}; i<8192; ++i, ++frame_buffer_index){
  //  frame_buffer[frame_buffer_index] = 0xffffffff;
  //}

  file.close();
  return true;
}

inline uint32_t arr_to_px(const char* buf){ //Only accepts char[4] please do not give me differently sized arrays
  return (
  ((buf[0]<<24) & 0xff00'0000) |
  ((buf[1]<<16) & 0x00ff'0000) |
  ((buf[2]<<8) & 0x0000'ff00) |
  ((buf[3]) & 0x0000'00ff)
);
}

inline int calc_index_hash(uint32_t pixel){ //Hash function for the index
  return ( 
  ((pixel & 0xff00'0000) /*r*/ >>24)*3 +
  ((pixel & 0x00ff'0000) /*g*/ >>16)*5 +
  ((pixel & 0x0000'ff00) /*b*/ >>8) *7 +
  ( pixel & 0x0000'00ff) /*a*/      *11
) % 64;
}

QOI_OP read_tag(const uint8_t tag){
  //Check for QOI_OP_RGB and QOI_OP_RGBA tags
  if(tag == 0b1111'1110){
    std::cout << "RGB";
    return QOI_OP::RGB;
  }
  if(tag == 0b1111'1111){
    std::cout << "RGBA";
    return QOI_OP::RGBA;
  }

  //Check for two bit tags
  if((tag>>6) == 0b0000'0011){
    std::cout << "RUN";
    return QOI_OP::RUN;
  }
  if((tag>>6) == 0b0000'0010){
    std::cout << "LUMA";
    return QOI_OP::LUMA;
  }
  if((tag>>6) == 0b0000'0001){
    std::cout << "DIFF";
    return QOI_OP::DIFF;
  }
  if((tag>>6) == 0b0000'0000){
    std::cout << "INDEX";
    return QOI_OP::INDEX;
  }

  return QOI_OP::MAX_TAG;
}

//TODO: Store pixel function
