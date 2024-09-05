#pragma once

#include <fstream>

#ifndef DONT_PRINT_HUFFMAN_TREE
#include <iostream>
#include <iomanip>
#endif

namespace HuffmanCoding {

   const size_t PAGE_SIZE = 0x1000;
   using uchar = unsigned char;
   using uint64 = unsigned long long;

   typedef struct TreeNode {
      uchar height;
      uchar character;
      uint64 frequency;

      TreeNode* l_child_node;
      TreeNode* r_child_node;
      TreeNode* next_node;
   } TreeNode;

   typedef struct CodeWord {
      uchar length;
      uchar* branches;
   } CodeWord;

   template <typename type>
   void print_bits(type num, size_t i) {
      for (i = 1ull << i - 1; i; i >>= 1)
         std::cout.put('0' + !!(num & i));
   }

   namespace InBitTools {
      inline bool __getbit(uchar* base, uchar pos) {
         return base[(pos & 0xF8) >> 3] & (1 << (~pos & 0x07));
      }

      inline void __flipbit(uchar* base, uchar pos) {
         base[(pos & 0xF8) >> 3] ^= 1 << (~pos & 0x07);
      }

      inline void __setbit_0(uchar* base, uchar pos) {
         base[(pos & 0xF8) >> 3] &= ~(1 << (pos & 0x07));
      }

      inline void __setbit_1(uchar* base, uchar pos) {
         base[(pos & 0xF8) >> 3] |= 1 << (pos & 0x07);
      }
   };

   bool encode(const char*, const char*);

   bool decode(const char* input_filename, const char* output_filename) {
      return false;
   }
}

#include "huffman.cpp"