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

   bool open_files(HUFFile::File&, HUFFile::File&, const char*, const char*);
   
   bool encode(const char*, const char*);

   namespace encoding {
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
         } character_buffer[256];
      } State; // Meant for passing stuff around, I might planify it later

      inline byte find_frequencies(State&);
      inline bool build_queue(State&);
      inline void build_tree(State&);
      inline bool build_header(State&);
   };

   bool decode(const char*, const char*);

   namespace decoding {
      typedef struct {
         HUFFile::File ifile;
         HUFFile::File ofile;

         Node* head;

         HUFFile::Flags flags;
         uint64_t size;
      } State; // Meant for passing stuff around, I might planify it later

      inline bool build_tree_branch(State&, Node**);
      inline bool build_tree_leaf(State&, Node**);

      inline void destroy_tree(Node*);
   };

   void print_queue(Node*);
   
   void print_tree(Node*);
   void print_inline_tree(Node*);
   void print_inline_tree_recusive_step(Node*);

   void print_queue(Node* head) {
      if (!head) {
         std::cout.write("{}\n", 3);
         return;
      }

      Node* node = head;

      while (true) {
         print_inline_tree(node);
         std::cout << ": " << node->frequency;

         node = node->next_node;
         if (!node) break;

         std::cout.write(", ", 2);
      }

      std::cout.put('\n');
   }

   void print_tree(Node* node) {
      if (!node) return;

      if (node->child[0] == node->child[1]) {
         print_character(node->character);
         return;
      }

      print_inline_tree_recusive_step(node->child[0]);
      print_inline_tree_recusive_step(node->child[1]);
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