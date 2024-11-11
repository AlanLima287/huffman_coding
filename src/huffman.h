#pragma once

#include <iostream>
#include <iomanip>

#include <stdint.h>
#include <stdio.h>

#include "system.h"
#include "../lib/bittools.h"

#define minimum(a, b) (((a) < (b)) ? (a) : (b))
#define maximum(a, b) (((a) > (b)) ? (a) : (b))

void print_char(char ch) {

   /*
   * Character printing convention:
   *
   *    num ∊ [0, 6]: "\{num}"
   *    num ∊ [7, D]:
   *       num = 7: "\a",
   *       num = 8: "\b",
   *       num = 9: "\t",
   *       num = A: "\n",
   *       num = B: "\v",
   *       num = C: "\f",
   *       num = D: "\r";
   *    num ∊ [E, 1B) ∪ (1B, 1F] ∪ [7F, FF]: "\x{num}"
   *    num ∊ (20, 7E]: (char)num
   *    num = 1B: "\e"
   *    num = 20: ' '
   */

   /* The following notes show the recorded branching behavior */

   /* [0, 19] ∪ [7F, FF] */
   if (0x20 > ch || ch >= 0x7f) {

      std::cout.put('\\');

      /* [0, 7) = [0, 6] */
      if (ch < 0x07) {
         std::cout << (short)(ch);
      }

      /* [7, E) = [7, D] */
      else if (ch < 0x0E) {
         std::cout.put("abtnvfr"[ch - 0x07]);
      }

      /* {1B} */
      else if (ch < 0x1B) {
         std::cout.put('e');
      }

      /* [E, 1B) ∪ (1B, 1F] ∪ [7F, FF] */
      else {
         std::cout << 'x' << std::hex << std::setw(2) << std::setfill('0')
            << (short)(ch);
      }
   }

   /* {20} */
   else if (ch == 0x20) {
      std::cout.write("' '", 3);
   }

   /* (20, 7F) = [21, 7E] */
   else {
      std::cout.put(ch);
   }
}

namespace HuffmanCoding {

   using byte = unsigned char;
   using intnode_t = unsigned short;

   const uint64_t PAGE_SIZE = 0x1000;

   typedef struct Node {
      uint64_t frequency;

      byte character;
      byte height;

      intnode_t left_child;
      intnode_t rght_child;
      intnode_t next_node;
   } Node;

   const intnode_t undefined = (intnode_t)-1;

   typedef union {
      byte state;
      byte
         true_tree : 1, // The file contains 2 or more kinds of characters
         truly_encoded : 1, // The file is encoded under the default encoding
         b5 : 1,
         b4 : 1,
         b3 : 1,
         size_order : 3; // The amount of bytes (-1) used to store the number of character in the original file
   } HUFFileFlags;

   typedef struct {
      FILE* file; // File Pointer
      uint64_t size; // Amount of loaded byte
      uint64_t length; // Length of the whole file
      uint64_t cursor; // A cursor inside the file buffer
      byte* buffer; // The file buffer
   } HUFIFile;

   bool getchar(HUFIFile&, byte&); // for encoding
   bool getbits(HUFIFile&, byte&); // for encoding

   typedef struct {
      FILE* file; // File Pointer
      uint64_t cursor; // A cursor inside the file buffer
      byte* buffer; // The file buffer
   } HUFOFile;

   void putchar(HUFOFile&, byte); // for encoding
   void putbits(HUFOFile&, byte); // for encoding

   typedef struct {
      HUFIFile hufifile;
      HUFOFile hufofile;

      Node* base;
      intnode_t head;
      intnode_t index;

      HUFFileFlags flags;
      uint64_t size;
   } State; // Meant for passing stuff around, I might planify it later

   bool find_frequencies(State&);
   void build_tree(State&);

   // Make the tree
   // Generate the codes
   // Encode the file

   bool encode(const char*, const char*);

   bool decode(const char* input_filename, const char* output_filename) {
      return false;
   }

   void print_queue(State&);
   void print_inline_tree(State&, intnode_t);
   void print_inline_tree_recusive_step(State&, intnode_t);

   void print_queue(State& state) {
      if (state.head == undefined) {
         std::cout.write("{}\n", 3);
         return;
      }

      intnode_t n = state.head;

      while (true) {
         print_inline_tree(state, n);
         std::cout << ": " << state.base[n].frequency;

         n = state.base[n].next_node;
         if (n == undefined) break;

         std::cout.write(", ", 2);
      }

      std::cout.put('\n');
   }

   void print_inline_tree(State& state, intnode_t index) {
      if (index == undefined) return;

      if (state.base[index].left_child == state.base[index].rght_child) {
         print_char(state.base[index].character);
         return;
      }

      print_inline_tree_recusive_step(state, state.base[index].left_child);
      print_inline_tree_recusive_step(state, state.base[index].rght_child);
   }

   void print_inline_tree_recusive_step(State& state, intnode_t index) {
      if (
         state.base[index].left_child == undefined &&
         state.base[index].rght_child == undefined)
      {
         print_char(state.base[index].character);
         return;
      }

      std::cout.put('(');

      if (state.base[index].left_child != undefined)
         print_inline_tree_recusive_step(state, state.base[index].left_child);

      if (state.base[index].rght_child != undefined)
         print_inline_tree_recusive_step(state, state.base[index].rght_child);

      std::cout.put(')');
   }
}

#include "huffman.cpp"