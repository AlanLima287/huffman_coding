#pragma once

#include <iostream>
#include <fstream>
#include "__int256.h"

#ifndef DONT_PRINT_HUFFMAN_TREE
#include <iomanip>
#endif

namespace HuffmanCoding {

   const size_t PAGE_SIZE = 0x1000;
   using uchar = unsigned char;

   typedef struct TreeNode {
      uchar height;
      uchar character;
      long frequency;

      TreeNode* l_child_node;
      TreeNode* r_child_node;
      TreeNode* next_node;
   } TreeNode;

   bool encode(const char*, const char*);

   bool decode(const char* input_filename, const char* output_filename) {
      return false;
   }

#ifndef DONT_PRINT_HUFFMAN_TREE
   const char branches[][3] = { "\xB3\x20", "\xC0\xC4", "\xC2\xC4", "\x20\x20" };
#endif

}

#include "huffman.cpp"