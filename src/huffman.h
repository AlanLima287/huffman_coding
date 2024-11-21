#pragma once

#include "base.h"
#include "huffile.h"
#include "../lib/bittools.h"

namespace HuffmanCoding {

   using intnode_t = unsigned short;

   typedef struct Node {
      uint64_t frequency;

      byte character;
      byte height;

      intnode_t left_child;
      intnode_t rght_child;
      intnode_t next_node;
   } Node;

   const intnode_t undefined = (intnode_t)-1;

   typedef struct {
      uint8_t length;
      byte* branch;
   } CodeWord;

   typedef struct {
      HUFFile::File ifile;
      HUFFile::File ofile;

      Node* base;
      intnode_t head;
      intnode_t index;

      uint16_t unique_chars;

      HUFFile::Flags flags;
      uint64_t size;

      CodeWord* connections;
      byte* branches;

      union {
         uint64_t count;
         CodeWord* code;
      } character_buffer[256] = {};
   } State; // Meant for passing stuff around, I might planify it later

   inline bool find_frequencies(State&);
   inline void build_tree(State&);
   inline void build_header(State&);
   inline void encode_file(State&);

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