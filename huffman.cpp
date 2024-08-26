#include "huffman.h"

bool HuffmanCoding::encode(const char* input_filepath, const char* output_filepath) {

   // Loads the input file
   std::fstream input_file(input_filepath, std::ios::in | std::ios::binary);
   if (!input_file) return false;

   long character_buffer[256] = {}; // Holds the count/code for every ASCII character
   size_t file_length = 0; // Stores the length of the file // Has not been useful yet

   TreeNode* head_pointer = nullptr; // It is the head of the priority queue

   size_t header_size = 0;
   uchar* header = nullptr; // Will point to the header, i.e., the encoded tree

   {
      uchar ch;

      // Reads the file character-wise incrementing for the corresponding
      // character and file length
      while (input_file.get((char&)ch)) {
         character_buffer[ch]++;
         file_length++;
      }

      // ch will now be used as an index
      ch = 0xff;

      do {
         // Checks whether the frequency is non-zero
         if (!character_buffer[ch]) continue;
         header_size += 8;

         if (!head_pointer || head_pointer->frequency >= character_buffer[ch]) {
            // Creates a new node an inserts it at the start of the queue
            head_pointer = new TreeNode{ 0, ch, character_buffer[ch], nullptr, nullptr, head_pointer };
            continue;
         }

         for (TreeNode* n = head_pointer; n; n = n->next_node) {
            if (!n->next_node || n->next_node->frequency >= character_buffer[ch]) {
               // Creates a new node an inserts it somewhere else at the queue
               n->next_node = new TreeNode{ 0, ch, character_buffer[ch], nullptr, nullptr, n->next_node };
               break;
            }
         }

         // Stops when ch is 0, if not 0, decreases it;
      } while (ch--);
   }

   // Building the Huffman tree
   {
      while (head_pointer && head_pointer->next_node) {

         //uchar weight = 1 + ((head_pointer->weight >= head_pointer->next_node->weight) ? head_pointer->weight : head_pointer->next_node->weight);
         uchar weight = 1 + head_pointer->weight + head_pointer->next_node->weight;

         long frequency = head_pointer->frequency + head_pointer->next_node->frequency;

         TreeNode* l_child;
         TreeNode* r_child;
         if (head_pointer->weight >= head_pointer->next_node->weight) {
            l_child = head_pointer;
            r_child = head_pointer->next_node;
         }
         else {
            l_child = head_pointer->next_node;
            r_child = head_pointer;
         }

         for (TreeNode* n = head_pointer->next_node; n; n = n->next_node) {
            if (!n->next_node || frequency < n->next_node->frequency ||
               (frequency == n->next_node->frequency && weight >= n->next_node->weight)) {

               n->next_node = new TreeNode{
                  weight, 0, frequency, l_child, r_child, n->next_node
               };

               break;
            }
         }

         head_pointer = head_pointer->next_node->next_node;
         l_child->next_node = nullptr;
         r_child->next_node = nullptr;
         header_size += 16;
      }
   }

   // Enconding the tree as a sequence of characters
   {
      header = new uchar[((header_size - 1) >> 3) + 2];

      size_t byte_cursor = 0;
      uchar bit_cursor = 6;

      while (head_pointer) {

         // do printing

         /*for (TreeNode* i = head_pointer; i; i = i->next_node) {
            if (0x20 <= i->character) std::cout << (char)i->character << '-';
            else std::cout << (int)i->character << '-';
         }
         std::cout << '\n';*/

         if (head_pointer->weight) {
            if (head_pointer->l_child_node->l_child_node) {
               header[byte_cursor++] = '1';
               
               if (head_pointer->r_child_node->l_child_node)
                  header[byte_cursor] = '1';
               else 
                  header[byte_cursor] = '0';
            }
            else {
               header[byte_cursor] = '0';
            }

            head_pointer->r_child_node->next_node = head_pointer->next_node;
            head_pointer->l_child_node->next_node = head_pointer->r_child_node;
            head_pointer->next_node = head_pointer->l_child_node;
         }
         else {
            header[byte_cursor] = head_pointer->character;
         }

         header[++byte_cursor] = 0;

         TreeNode* tmp = head_pointer->next_node;
         delete head_pointer;
         head_pointer = tmp;
      }

      std::cout << header << '\n';

      /* while (head_pointer->r_child_node) {

         // Moved down-left printing directional tags until a leaf node
         if (pointer->l_child_node) {
            header[byte_cursor] |= pointer->character << bit_cursor;

            if (!bit_cursor) {
               header[++byte_cursor] = 0;
               bit_cursor = 6;
            }
            else bit_cursor -= 2;

            pointer = pointer->l_child_node;
         }
         else {
            // print character to header
            if (pointer->weight == 0) {

               // Inserts the first half of the character
               header[byte_cursor] |= pointer->character >> (6 - bit_cursor);
               header[++byte_cursor] = 0;

               // Inserts the second half of the character if there is some
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
      } */
   }

   return true;
}