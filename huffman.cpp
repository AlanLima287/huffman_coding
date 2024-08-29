#include "huffman.h"

void print_tree(HuffmanCoding::TreeNode* head, HuffmanCoding::uchar tree_height, HuffmanCoding::uchar height, size_t& connections) {
   using namespace HuffmanCoding;

   if (!head->height) {

      for (uchar i = tree_height - height; i < tree_height; i++)
         std::cout << "  ";

      std::cout.put(head->character);
      std::cout.put(0xC4);

      bool zero_break = true;
      bool one_break = true;

      for (uchar i = height; i < tree_height; i++) {
         if (connections & (1ull << i)) {
            if (zero_break) {
               std::cout.put(0xC4);
               std::cout.put(0xC2);
               connections ^= 1ull << i;
            }
            else {
               std::cout.put(0x20);
               std::cout.put(0x20);
            }
         }
         else {
            if (one_break) {
               std::cout.put(0xC4);
               std::cout.put(0xD9);

               connections ^= 1ull << i;

               zero_break = false;
               one_break = false;
            }
            else {
               std::cout.put(0x20);
               std::cout.put(0xB3);
            }
         }
      }

      std::cout.put('\n');
   }

   if (head->l_child_node) print_tree(head->l_child_node, tree_height, height - 1, connections);
   if (head->r_child_node) print_tree(head->r_child_node, tree_height, height - 1, connections);
}

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

         uchar height = 1 + ((head_pointer->height >= head_pointer->next_node->height) ? head_pointer->height : head_pointer->next_node->height);
         //uchar height = 1 + head_pointer->height + head_pointer->next_node->height;

         long frequency = head_pointer->frequency + head_pointer->next_node->frequency;

         TreeNode* l_child;
         TreeNode* r_child;
         if (head_pointer->height >= head_pointer->next_node->height) {
            l_child = head_pointer;
            r_child = head_pointer->next_node;
         }
         else {
            l_child = head_pointer->next_node;
            r_child = head_pointer;
         }

         for (TreeNode* n = head_pointer->next_node; n; n = n->next_node) {
            if (!n->next_node || frequency < n->next_node->frequency ||
               (frequency == n->next_node->frequency && height < n->next_node->height)) {

               n->next_node = new TreeNode{
                  height, uchar((!!l_child->height << 1) | !!r_child->height), frequency, l_child, r_child, n->next_node
               };

               break;
            }
         }

         head_pointer = head_pointer->next_node->next_node;
         header_size += head_pointer->character;
         l_child->next_node = nullptr;
         r_child->next_node = nullptr;
      }
   }

   size_t connections = ~0;
   //connections >>= sizeof(size_t) - head_pointer->height;
   print_tree(head_pointer, head_pointer->height, head_pointer->height, connections);

   // Enconding the tree as a sequence of characters
   {
      header_size = ((header_size - 1) >> 3) + 2; // From bits to bytes
      header = new uchar[sizeof(size_t) + header_size];

      size_t byte_cursor = 0;
      uchar bit_cursor = 7;

      *(size_t*)header = file_length;

      byte_cursor = sizeof(size_t) + header_size;
      for (size_t i = 0; i < header_size; i++) {
         header[byte_cursor--] = 0;
      }

      while (head_pointer) {

         // for (TreeNode* i = head_pointer; i; i = i->next_node) {
         //    if (0x20 <= i->character) std::cout << (char)i->character << '-';
         //    else std::cout << (int)i->character << '-';
         // }
         // std::cout << '\n';

         if (head_pointer->height) {
            if (head_pointer->l_child_node->height) {
               header[byte_cursor] |= 1 << bit_cursor;

               if (!bit_cursor) { bit_cursor = 8; byte_cursor++; }
               bit_cursor--;

               if (head_pointer->r_child_node->l_child_node)
                  header[byte_cursor] |= 1 << bit_cursor;
            }

            if (!bit_cursor) { bit_cursor = 8; byte_cursor++; }
            bit_cursor--;

            head_pointer->r_child_node->next_node = head_pointer->next_node;
            head_pointer->l_child_node->next_node = head_pointer->r_child_node;
            head_pointer->next_node = head_pointer->l_child_node;
         }
         else {
            header[byte_cursor++] |= head_pointer->character >> (7 - bit_cursor);
            header[byte_cursor] |= head_pointer->character << (1 + bit_cursor);
         }

         TreeNode* tmp = head_pointer->next_node;
         delete head_pointer;
         head_pointer = tmp;
      }

      // std::cout << header << '\n';
   }

   return true;
}