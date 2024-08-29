#include "huffman.h"

bool HuffmanCoding::encode(const char* input_filename, const char* output_filename) {

   // Loads the input file
   std::fstream input_file(input_filename, std::ios::in | std::ios::binary);
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

         uchar height = 1 + (head_pointer->height >= head_pointer->next_node->height ? head_pointer->height : head_pointer->next_node->height);
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

#ifndef DONT_PRINT_HUFFMAN_TREE
   // Prints the tree to the terminal. Optinal

   connections = ~0;
   entropy = 0;

   print_tree(head_pointer);
   std::cout << "Entropy: " << (float)entropy / file_length << '\n';
#endif

   // Enconding the tree as a sequence of characters
   {
      header_size = (header_size - 1 >> 3) + 2; // From bits to bytes
      header = new uchar[sizeof(size_t) + header_size];

      // *(size_t*)header = file_length;
      for (size_t i = 0; i < sizeof(size_t); i++)
         *header++ = file_length >> (i << 3);

      for (size_t i = 0; i < header_size; i++)
         header[i] = 0;

      header -= sizeof(size_t);
      size_t byte_cursor = 0;
      uchar bit_cursor = 7;

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

#ifndef DONT_PRINT_HUFFMAN_TREE
void HuffmanCoding::print_tree(TreeNode* head, uchar height) {
   if (!head->height) {

      __int256 comparator = connections;
      for (uchar i = height - 1; i < 0xff; i--) {
         if (connections.__getbit(i))
            comparator.__setbit_0(i);
         else {
            comparator.__setbit_1(i);
            break;
         }
      }

      char branches[][3] = { "\xB3\x20", "\xC0\xC4", "\xC2\xC4", "\x20\x20" };
      for (uchar i = 0; i < height; i++) {
         std::cout << branches[connections.__getbit(i) << 1 | comparator.__getbit(i)];
      }

      if (0x20 < head->character && head->character < 0x7f)
         std::cout.put(head->character);
      else if (head->character == 0x20)
         std::cout << "Space";
      else
         std::cout << "0x" << std::hex << (short)(head->character);


      std::cout.put('\n');

      entropy += height * head->frequency;
      connections = comparator;
   }

   if (head->l_child_node) print_tree(head->l_child_node, height + 1);
   if (head->r_child_node) print_tree(head->r_child_node, height + 1);
}
#endif