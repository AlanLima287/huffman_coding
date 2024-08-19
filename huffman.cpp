#include <iostream>
#include <fstream>

namespace HuffmanCoding {

   typedef unsigned char uchar;

   struct TreeNode {
      uchar character;
      long frequency;

      TreeNode* parent_node;
      TreeNode* l_child_node;
      TreeNode* r_child_node;
   };

   struct Node {
      TreeNode* content;
      Node* next_node;
   };

   bool encode(const char* input_filepath, const char* output_filepath) {

      // Loads the input file
      std::fstream input_file(input_filepath, std::ios::in | std::ios::binary);
      if (!input_file) return false;

      long character_count[256] = {}; // Holds the count of every ASCII character

      Node* head_pointer = nullptr; // Points to the front of the queue
      size_t file_length = 0; // Stores the length of the file // Has not been useful yet
      size_t header_size = 0;

      {
         unsigned char ch;

         // Reads the file character-wise incrementing for the corresponding
         // character and file length
         while (input_file.get((char&)ch)) {
            character_count[ch]++;
            file_length++;
         }

         // ch will now be used as an index
         ch = 0;

         do {
            // Checks whether the frequency is non-zero
            if (!character_count[ch]) continue;
            header_size += 8;

            if (!head_pointer || head_pointer->content->frequency >= character_count[ch]) {
               // Creates a new node an inserts it at the start of the queue
               head_pointer = new Node{ new TreeNode{ ch, character_count[ch], 0, 0, 0 }, head_pointer };
               continue;
            }

            for (Node* n = head_pointer; n; n = n->next_node) {
               if (!n->next_node || n->next_node->content->frequency >= character_count[ch]) {
                  // Creates a new node an inserts it somewhere else at the queue
                  n->next_node = new Node{ new TreeNode{ ch, character_count[ch], 0, 0, 0 }, n->next_node };
                  break;
               }
            }

            // Uses an overflow of the 8bit variable ch to stop. Also, it increments
            // before checking
         } while (++ch);
      }

      while (head_pointer && head_pointer->next_node) {

         uchar character = 0;
         if (head_pointer->next_node->content->l_child_node) character |= 0b10;
         if (head_pointer->content->l_child_node) character |= 0b01;

         TreeNode* node = new TreeNode{
            character,
            head_pointer->content->frequency + head_pointer->next_node->content->frequency,
            nullptr,
            head_pointer->next_node->content,
            head_pointer->content,
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

         header_size += 2;
      }

      uchar* header = new uchar[((header_size - 1) >> 3) + 2];

      {
         TreeNode* pointer = head_pointer->content;

         size_t byte_cursor = 0;
         uchar bit_cursor = 6;
         header[0] = 0;

         while (head_pointer->content->r_child_node) {

            // Moved down-left printing directional tags until a leaf node
            if (pointer->l_child_node) {
               header[byte_cursor] |= pointer->character << bit_cursor;
               pointer->frequency = 0;

               if (!bit_cursor) {
                  header[++byte_cursor] = 0;
                  bit_cursor = 6;
               }
               else bit_cursor -= 2;

               pointer = pointer->l_child_node;
            }
            else {
               // print character to header
               if (pointer->frequency) {

                  // Inserts the first half of the character
                  header[byte_cursor] |= pointer->character >> (6 - bit_cursor);
                  header[++byte_cursor] = 0;

                  // Inserts the second half of the character
                  if (bit_cursor != 6) {
                     header[byte_cursor] = pointer->character << (2 + bit_cursor);
                  }
               }

               // Move to its right sibling if it isn't already the right sibling
               // Also frees up the its left sibling
               if (pointer->parent_node->r_child_node && pointer->parent_node->r_child_node != pointer) {
                  pointer = pointer->parent_node->r_child_node;

                  delete pointer->parent_node->l_child_node;
                  pointer->parent_node->l_child_node = nullptr;
               }

               // Move to the parent if both child are null pointers
               // Frees up the right sibling
               else {
                  pointer = pointer->parent_node;

                  delete pointer->r_child_node;
                  pointer->r_child_node = nullptr;
               }
            }
         }

         delete head_pointer->content;
         delete head_pointer;
      }

      return true;
   }

   bool decode(const char* input_filepath, const char* output_filepath) {
      return false;
   }
}
