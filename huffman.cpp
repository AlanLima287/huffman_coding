#include <iostream>
#include <fstream>

namespace HuffmanCoding {

   typedef struct TreeNode {
      char character;
      long frequency;

      TreeNode* parent_node;
      TreeNode* l_child_node;
      TreeNode* r_child_node;
   };

   typedef struct Node {
      TreeNode* content;
      Node* next_node;
   };

   bool encode(const char* input_filepath, const char* output_filepath) {

      std::fstream input_file(input_filepath, std::ios::in | std::ios::binary);
      if (!input_file) return false;

      long character_count[256] = {};

      Node* head_pointer = nullptr;
      size_t file_length = 0;

      {
         unsigned char ch;

         while (input_file.get((char&)ch)) {
            character_count[ch]++;
            file_length++;
         }

         ch = 0;

         do {
            if (!character_count[ch]) continue;

            if (!head_pointer) {
               head_pointer = new Node{ new TreeNode{ (char)ch, character_count[ch], 0, 0, 0 }, nullptr };
               break;
            }
         } while (++ch);

         ch++;

         do {
            if (!character_count[ch]) continue;

            if (head_pointer->content->frequency >= character_count[ch]) {
               head_pointer = new Node{ new TreeNode{ (char)ch, character_count[ch], 0, 0, 0 }, head_pointer };
               continue;
            }

            for (Node* n = head_pointer; n; n = n->next_node) {
               if (!n->next_node || n->next_node->content->frequency >= character_count[ch]) {
                  n->next_node = new Node{ new TreeNode{ (char)ch, character_count[ch], 0, 0, 0 }, n->next_node };
                  break;
               }
            }

         } while (++ch);
      }

      while (head_pointer && head_pointer->next_node) {
         TreeNode* node = new TreeNode{
            0,
            head_pointer->content->frequency + head_pointer->next_node->content->frequency,
            nullptr,
            head_pointer->content,
            head_pointer->next_node->content
         };

         head_pointer->next_node->content->parent_node = node;
         head_pointer->content->parent_node = node;

         for (Node* n = head_pointer->next_node; n; n = n->next_node) {
            if (!n->next_node || n->next_node->content->frequency >= node->frequency) {
               n->next_node = new Node{ node, n->next_node };
               break;
            }
         }

         Node* tmp = head_pointer->next_node->next_node;

         delete head_pointer->next_node;
         delete head_pointer;

         head_pointer = tmp;
      }

      TreeNode root = *head_pointer->content;
      delete head_pointer;

      return true;
   }

   bool decode(const char* input_filepath, const char* output_filepath) {
      return false;
   }
}
