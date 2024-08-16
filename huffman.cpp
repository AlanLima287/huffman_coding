#include <iostream>
#include <fstream>

#define max(a, b) ((a) > (b) ? (a) : (b))

namespace HuffmanCoding {

   typedef struct TreeNode {
      short height;

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
      sizeof(TreeNode);

      // Loads the input file
      std::fstream input_file(input_filepath, std::ios::in | std::ios::binary);
      if (!input_file) return false;

      long character_count[256] = {}; // Holds the count of every ASCII character

      Node* head_pointer = nullptr; // Points to the front of the queue
      size_t file_length = 0; // Stores the length of the file // Has not been useful yet

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

            if (!head_pointer || head_pointer->content->frequency >= character_count[ch]) {
               // Creates a new node an inserts it at the start of the queue
               head_pointer = new Node{ new TreeNode{ 0, (char)ch, character_count[ch], 0, 0, 0 }, head_pointer };
               continue;
            }

            for (Node* n = head_pointer; n; n = n->next_node) {
               if (!n->next_node || n->next_node->content->frequency >= character_count[ch]) {
                  // Creates a new node an inserts it somewhere else at the queue
                  n->next_node = new Node{ new TreeNode{ 0, (char)ch, character_count[ch], 0, 0, 0 }, n->next_node };
                  break;
               }
            }

            // Uses an overflow of the 8bit variable ch to stop. Also, it increments
            // before checking
         } while (++ch);
      }

      Node* holder = head_pointer;

      while (head_pointer && head_pointer->next_node) {

         short height;
         TreeNode* l_child;
         TreeNode* r_child;

         if (head_pointer->content->height < head_pointer->next_node->content->height) {
            height = head_pointer->next_node->content->height + 1;
            
            l_child = head_pointer->next_node->content;
            r_child = head_pointer->content;
         } else {
            height = head_pointer->content->height + 1;

            r_child = head_pointer->next_node->content;
            l_child = head_pointer->content;
         }

         TreeNode* node = new TreeNode{
            height, 0,
            head_pointer->content->frequency + head_pointer->next_node->content->frequency,
            nullptr, l_child, r_child
         };

         head_pointer->next_node->content->parent_node = node;
         head_pointer->content->parent_node = node;

         for (Node* n = head_pointer->next_node; n; n = n->next_node) {
            if (!n->next_node || n->next_node->content->frequency >= node->frequency) {
               n->next_node = new Node{ node, n->next_node };
               break;
            }
         }

         head_pointer = head_pointer->next_node->next_node;

         //Node* tmp = head_pointer->next_node->next_node;

         //delete head_pointer->next_node;
         //delete head_pointer;

         //head_pointer = tmp;
      }

      TreeNode root = *head_pointer->content;
      //delete head_pointer;

      return true;
   }

   bool decode(const char* input_filepath, const char* output_filepath) {
      return false;
   }
}
