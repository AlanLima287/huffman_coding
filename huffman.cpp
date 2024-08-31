#include "huffman.h"

bool HuffmanCoding::encode(const char* input_filename, const char* output_filename) {

   // Loads the input file
   std::fstream input_file(input_filename, std::ios::in | std::ios::binary);
   if (!input_file) return false;

   long character_buffer[256] = {}; // Holds the count/code for every ASCII character
   size_t file_length = 0; // Stores the length of the file

   TreeNode* head_pointer = nullptr; // It is the head of the priority queue

   size_t header_size = 0;
   uchar* header = nullptr; // Will point to the header, i.e., the encoded tree

   {
      uchar ch;

      // Reads the file character-wise incrementing for the corresponding
      // character frequency and file length
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
            if (!n->next_node || frequency < n->next_node->frequency || (frequency == n->next_node->frequency && height < n->next_node->height)) {

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

   // Enconding the tree as a sequence of characters
   {
      header_size = (header_size - 1 >> 3) + 1; // From bits to bytes
      header = new uchar[sizeof(file_length) + header_size];

      // *(size_t*)header = file_length;
      // Garanties the Little Endian ordering, unlike above
      for (size_t i = 0; i < sizeof(file_length); i++)
         *header++ = file_length >> (i << 3);

      // Initialize the whole header to NULL bytes
      // Necessary for future bitwise or operatios
      for (size_t i = 0; i < header_size; i++)
         header[i] = 0;

      header -= sizeof(file_length);
      size_t byte_cursor = 0; // Index of the writing head of the header
      uchar bit_cursor = 7; // Index in-byte of the writing head of the header

      uchar depth = 0; // Stores the relative depth of the tree

      __int256 connections = ~0; // Don't even bother
      size_t entropy = 0; // Stores the Shannon entropy of the Huffman tree

      // The head_pointer will now be used as a stack top
      while (head_pointer) {

         if (head_pointer->height) {
            if (head_pointer->l_child_node->height) {
               // If the current node has left child as a leaf, put 1
               header[byte_cursor] |= 1 << bit_cursor;

               if (!bit_cursor) { bit_cursor = 8; byte_cursor++; }
               bit_cursor--;

               if (head_pointer->r_child_node->l_child_node)
                  // If the current node has right child as a leaf, put 1
                  header[byte_cursor] |= 1 << bit_cursor;
               // Else, a 0 is implicitily put, the same is valid for the outer if
            }

            if (!bit_cursor) { bit_cursor = 8; byte_cursor++; }
            bit_cursor--;

            // Pointer jugling, take your time (Sets up for poping the stack later)
            //                                    ┌───┐
            //    [h] → [n]    ┌──── [h]   [n]   [h]  ↓
            //  ┌──┴──┐     => │   ┌──┴──┐  ↑  =  │  [l] → [r] → [n]
            // [l]   [r]       └→ [l] → [r] ┘     └───┴─────┘
            //
            head_pointer->r_child_node->next_node = head_pointer->next_node;
            head_pointer->l_child_node->next_node = head_pointer->r_child_node;
            head_pointer->next_node = head_pointer->l_child_node;

            depth++; // If a tree has branched (has children), the depth is incremented
         }
         else {
            header[byte_cursor++] |= head_pointer->character >> (7 - bit_cursor); // First part of the character
            header[byte_cursor] |= head_pointer->character << (1 + bit_cursor); // Second part of the character

            __int256 comparator = connections;
            uchar old_depth = depth; // Stores the old depth
            uchar i = depth - 1; // Iterator variable

            // Well, it works! I might explain it later
            if (!connections.__getbit(i)) {
               comparator.__setbit_1(i);

               do {
                  depth--; i--;
               } while (connections.__getbit(i) && i);
            }
            else do {

               if (connections.__getbit(i))
                  comparator.__setbit_0(i);
               else {
                  comparator.__setbit_1(i);
                  break;
               }

            } while (i--);

            // Print the Huffman tree to the standart output
            {
               static const char branches[] = { '\xB3', '\x20', '\xC0', '\xC4', '\xC2', '\xC4', '\x20', '\x20' };

               i = 0;
               while (i < old_depth) {
                  std::cout.write(&branches[(int)connections.__getbit(i) << 2 | (int)comparator.__getbit(i) << 1], 2);
                  i++;
               }

               /* Character printing convention:

                  num ∊ [0, 6]: "\{num}";
                  num ∊ [7, D]:
                     num = 7: "\a",
                     num = 8: "\b",
                     num = 9: "\t",
                     num = A: "\n",
                     num = B: "\v",
                     num = C: "\f",
                     num = D: "\r";
                  num ∊ [E, 19] ∪ [7F, FF]: "\x{num}";
                  num ∊ (20, 7E]: (char)num;
                  num = 20: "Space".
               */

               // The following notes show the recorded branching behavior 
               if (0x20 > head_pointer->character || head_pointer->character >= 0x7f) {
                  // [0, 19] ∪ [7F, FF]

                  std::cout.put('\\');
                  if (head_pointer->character < 0x07)
                     // [0, 7) = [0, 6]
                     std::cout << (short)(head_pointer->character);
                  else if (head_pointer->character < 0x0E)
                     // [7, E) = [7, D]
                     std::cout.put("abtnvfr"[head_pointer->character - 0x07]);
                  else
                     // [E, 19] ∪ [7F, FF]
                     std::cout << 'x' << std::hex << std::uppercase << std::setw(2)
                     << std::setfill('0') << (short)(head_pointer->character);
               }
               else if (head_pointer->character == 0x20)
                  // {20}
                  std::cout << "Space";
               else
                  // (20, 7F) = [21, 7E]
                  std::cout.put(head_pointer->character);

               std::cout.put('\n');
            }

            entropy += old_depth * head_pointer->frequency; // ∑L(x)p(x)
            connections = comparator;
            // #endif
         }

         // Pops the stack, i.e., the head_pointer
         TreeNode* tmp = head_pointer->next_node;
         delete head_pointer;
         head_pointer = tmp;
      }

      std::cout << "Entropy: " << (float)entropy / file_length << '\n';
   }

   return true;
}