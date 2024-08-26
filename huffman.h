#pragma once

#include <iostream>
#include <fstream>

namespace HuffmanCoding {

   const size_t PAGE_SIZE = 0x1000;
   using uchar = unsigned char;

   struct TreeNode {
      uchar weight;
      uchar character;
      long frequency;

      TreeNode* l_child_node;
      TreeNode* r_child_node;
      TreeNode* next_node;
   };

   bool encode(const char*, const char*);

   bool decode(const char* input_filepath, const char* output_filepath) {
      return false;
   }
}

#include "huffman.cpp"