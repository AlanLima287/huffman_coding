#pragma once

#include <iostream>
#include <fstream>
#include "__int256.h"

namespace HuffmanCoding {

   const size_t PAGE_SIZE = 0x1000;
   using uchar = unsigned char;

   struct TreeNode {
      uchar height;
      uchar character;
      long frequency;

      TreeNode* l_child_node;
      TreeNode* r_child_node;
      TreeNode* next_node;
   };

   bool encode(const char*, const char*);

   bool decode(const char* input_filename, const char* output_filename) {
      return false;
   }

#ifndef DONT_PRINT_HUFFMAN_TREE
   __int256 connections;
   size_t entropy;

   void print_tree(TreeNode*, uchar = 0);
#endif

}

#include "huffman.cpp"