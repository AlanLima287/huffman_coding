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

   bool encode(const char*, const char*, bool = false);

   namespace encoding {
      typedef struct {
         HUFFile::File ifile;
         HUFFile::File ofile;

         Node* head;

         uint16_t unique_chars;

         HUFFile::Flags flags;

         uint64_t encoded_size;
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

   bool decode(const char*, const char*, bool = false);

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

   namespace application {

      void print_tree(Node*);
      void print_tree_recusive_step(Node*, byte[2][32], byte);

      /* deprecated */
      void print_queue(Node*);
      void print_inline_tree(Node*);
      void print_inline_tree_recusive_step(Node*);

      void print_tree(Node* node) {
         if (!node) {
            std::cout.write("{}\n", 3);
            return;
         }

         byte branches[2][32] = {};
         print_tree_recusive_step(node, branches, 0);
      }

      void print_tree_recusive_step(Node* node, byte branches[2][32], byte depth) {

         if (node->child[0] != node->child[1]) {

            BitTools::setbit_1(branches[0], depth);

            print_tree_recusive_step(node->child[0], branches, depth + 1);
            print_tree_recusive_step(node->child[1], branches, depth + 1);

         } else {

            for (byte i = 0; i < 32; i++)
               branches[1][i] = branches[0][i];

            byte i = depth - 1;
            bool bit = BitTools::flipbit(branches[0], i);

            if (i) {
               if (bit) while (BitTools::flipbit(branches[0], --i) && i);
               else while (BitTools::getbit(branches[0], --i) && i);
            }

            for (byte i = 0; i < depth; i++) {
               if (BitTools::getbit(branches[0], i)) {
                  if (BitTools::getbit(branches[1], i))
                     std::cout.write("\u0020\u0020\u0020", 3); else
                     std::cout.write("\u2514\u2500\u2500", 9);
               } else {
                  if (BitTools::getbit(branches[1], i))
                     std::cout.write("\u252C\u2500\u2500", 9); else
                     std::cout.write("\u2502\u0020\u0020", 5);
               }
            }

            std::cout.write(": ", 2);
            print_character(node->character);

            std::cout.put('\n');
         }
      }

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
   };
}