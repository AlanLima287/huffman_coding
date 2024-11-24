#pragma once

#include "base.h"
#include "huffile.h"
#include "../lib/bittools.h"

namespace HuffmanCoding {

   enum branch_t : byte { no_leaves, one_leaf, two_leaves };

   typedef struct Node {
      uint64_t frequency;

      Node* next_node;
      Node* child[2];

      byte character;
      byte height;
   } Node;

   typedef struct {
      uint8_t length;
      byte* branch;
   } CodeWord;

   typedef struct {
      HUFFile::File ifile;
      HUFFile::File ofile;

      Node* head;

      uint16_t unique_chars;

      HUFFile::Flags flags;
      uint64_t size;

      Node* base;
      CodeWord* connections;
      byte* branches;

      union {
         uint64_t count;
         CodeWord* code;
      } character_buffer[256] = {};
   } State; // Meant for passing stuff around, I might planify it later

   bool open_files(State&, const char*, const char*);

   bool encode(const char*, const char*);

   namespace encoding {
      inline byte find_frequencies(State&);
      inline bool build_queue(State&);
      inline void build_tree(State&);
      inline bool build_header(State&);
   };

   bool decode(const char*, const char*);

   namespace decoding {
      inline bool build_tree(State&);
      
      inline bool build_tree_branch(State&, Node**);
      inline bool build_tree_leaf(State&, Node**);

      inline bool get_character(State&, byte&);
   };


   void print_queue(State&);
   void print_inline_tree(Node*);
   void print_inline_tree_recusive_step(Node*);

   void print_queue(State& state) {
      if (!state.head) {
         std::cout.write("{}\n", 3);
         return;
      }

      Node* node = state.head;

      while (true) {
         print_inline_tree(node);
         std::cout << ": " << node->frequency;

         node = node->next_node;
         if (!node) break;

         std::cout.write(", ", 2);
      }

      std::cout.put('\n');
   }

   void print_inline_tree(Node* node) {
      if (!node) return;

      if (node->child[0] == node->child[1]) {
         print_character(node->character);
         return;
      }

      print_inline_tree_recusive_step(node->child[0]);
      print_inline_tree_recusive_step(node->child[1]);
   }

   void print_inline_tree_recusive_step(Node* node) {
      
      if (!node->child[0] && !node->child[1]) {
         print_character(node->character);
         return;
      }

      std::cout.put('(');

      if (node->child[0])
         print_inline_tree_recusive_step(node->child[0]);

      if (node->child[1])
         print_inline_tree_recusive_step(node->child[1]);

      std::cout.put(')');
   }
}

#include "huffman.cpp"