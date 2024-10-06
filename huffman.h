#pragma once

#include <fstream>
#include <iostream>
#include <iomanip>

namespace HuffmanCoding {

   // const size_t PAGE_SIZE = 0x1000;
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

   typedef union HUFFileFlags {
      uchar state;
      uchar
         true_tree : 1, // The file contains 2 or more kinds of characters
         truly_encode : 1, // The file is encoded under the default encoding
         b5 : 1,
         b4 : 1,
         b3 : 1,
         size_order : 3; // The amount of bytes used to store the number of character in the original file
   } HUFFileFlags;

   typedef struct CodeWord {
      uchar length;
      uchar* branches;
   } CodeWord;

   typedef union Buffer {
      CodeWord code;
      uint64 count;
   } Buffer;

   bool encode(const char*, const char*);

   bool decode(const char* input_filename, const char* output_filename) {
      return false;
   }
}

namespace InBitTools {
   
   template <typename type>
   void print_bits(type value, size_t bits) {
      for (bits = 1ull << bits - 1; bits; bits >>= 1)
         std::cout.put('0' + !!(value & bits));
   }

   inline bool __getbit(unsigned char* base, unsigned char pos) {
      return base[(pos & 0xF8) >> 3] & (1 << (~pos & 0x07));
   }

   inline void __flipbit(unsigned char* base, unsigned char pos) {
      base[(pos & 0xF8) >> 3] ^= 1 << (~pos & 0x07);
   }

   inline void __setbit_0(unsigned char* base, unsigned char pos) {
      base[(pos & 0xF8) >> 3] &= ~(1 << (pos & 0x07));
   }

   inline void __setbit_1(unsigned char* base, unsigned char pos) {
      base[(pos & 0xF8) >> 3] |= 1 << (pos & 0x07);
   }
};

#include "huffman.cpp"