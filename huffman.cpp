#include "huffman.h"

bool HuffmanCoding::encode(const char* input_filename, const char* output_filename) {

   // Loads the input file
   std::fstream input_file(input_filename, std::ios::in | std::ios::binary);
   if (!input_file) return false;

   // Loads the output file
   if (false) {
      std::fstream output_file(output_filename, std::ios::out | std::ios::binary);
      if (!output_file) return true;
   }

   // Holds the count/code for every ASCII character
   // Don't do this, unless you know what you are doing
   // I do not, but I'll do it regardless :)
   // Evil pointer trick!
   union Buffer {
      CodeWord code;
      uint64 count;
   } character_buffer[256] = {};

   // uchar* buffer = nullptr; // character_buffer's code variables will point to objects on this buffer
   CodeWord placeholder = {};

   uint64 file_length = 0; // Stores the length of the file
   uint64 entropy = 0; // Stores the (scaled by file_lenght) Shannon entropy of the Huffman tree

   TreeNode* head_pointer = nullptr; // It is the head of the priority queue

   unsigned short unique_characters = 0;
   uint64 header_size = 0;
   uchar* header = nullptr; // Will point to the header, i.e., the encoded tree

   {
      uchar ch;

      // Reads the file character-wise incrementing for the corresponding
      // character frequency and file length
      while (input_file.get((char&)ch)) {
         character_buffer[ch].count++;
         file_length++;
      }

      // ch will now be used as an index
      ch = 0xff;

      do {
         // Checks whether the frequency is non-zero
         if (!character_buffer[ch].count) continue;
         unique_characters++;

         if (!head_pointer || head_pointer->frequency >= character_buffer[ch].count) {
            // Creates a new node [N] and inserts it at the start of the queue
            //
            // [h] → ... => [N] → [h] → ...
            // [N]
            //
            head_pointer = new TreeNode{ 0, ch, character_buffer[ch].count, nullptr, nullptr, head_pointer };
            continue;
         }

         for (TreeNode* n = head_pointer; n; n = n->next_node) {
            if (!n->next_node || n->next_node->frequency >= character_buffer[ch].count) {
               // Creates a new node [N] an inserts it somewhere else at the queue
               //
               // [h] → ... → [n] → [m] → ... => [h] → ... → [n] ─/→ [m] = [h] → ... → [n] → [N] → [m] → ...
               //             [N]                             └→ [N]  ↑
               //                                                 └───┘
               n->next_node = new TreeNode{ 0, ch, character_buffer[ch].count, nullptr, nullptr, n->next_node };
               break;
            }
         }

         // Stops when ch is 0, if not 0, decreases it;
      } while (ch--);

      header_size = unique_characters << 3;
   }

   // Building the Huffman tree
   {
      while (head_pointer && head_pointer->next_node) {

         uchar height = 1 + (head_pointer->height >= head_pointer->next_node->height ? head_pointer->height : head_pointer->next_node->height);
         //uchar height = 1 + head_pointer->height + head_pointer->next_node->height;

         uint64 frequency = head_pointer->frequency + head_pointer->next_node->frequency;

         // Garantees that the right child will never be higher than the left child
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
                  height, (uchar)('0' + ((!!l_child->height << 1) | !!r_child->height)), frequency, l_child, r_child, n->next_node
               };

               break;
            }
         }

         head_pointer = head_pointer->next_node->next_node;
         header_size += head_pointer->character - '0';
         l_child->next_node = nullptr;
         r_child->next_node = nullptr;
      }
   }

   // Enconding the tree as a sequence of characters
   {
      header_size = ((header_size - 1) >> 3) + 1; // From bits to bytes
      header = new uchar[sizeof(file_length) + header_size];

      // *(uint64*)header = file_length;
      // Does garantee the Little Endian ordering, unlike above
      for (uint64 i = 0; i < sizeof(file_length); i++)
         *header++ = file_length >> (i << 3);

      // Initialize the whole header to NULL bytes
      // Necessary for future bitwise or operatios
      for (uint64 i = 0; i < header_size; i++)
         header[i] = 0;

      header -= sizeof(file_length);
      uint64 byte_cursor = 0; // Index of the writing head of the header
      uchar bit_cursor = 7; // Index in-byte of the writing head of the header

      uchar length = ((head_pointer->height - 1) >> 3) + 1; // Stores the height of the tree in bytes
      uchar depth = 0; // Stores the relative depth of the tree

      placeholder.branches = new uchar[unique_characters * length];
      for (uchar i = 0; i < length; i++) {
         placeholder.branches[i] = 0xff;
      }

      CodeWord* connections = &placeholder;

      // The head_pointer will now be used as a stack top
      while (head_pointer) {

         // for (TreeNode* n = head_pointer; n; n = n->next_node) {
         //    std::cout.put(n->character);
         //    std::cout.put('-');
         // }
         // std::cout.put('\n');

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

            // Pointer jugling, take your time (Sets up for popping the stack later)
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

            character_buffer[head_pointer->character].code.branches = connections->branches + length;
            character_buffer[head_pointer->character].code.length = depth;

            uchar i; // Iterator variable
            for (i = 0; i < length; i++)
               character_buffer[head_pointer->character].code.branches[i] = connections->branches[i];

            i = depth - 1;

            // Well, it works! I might explain it later
            InBitTools::__flipbit(character_buffer[head_pointer->character].code.branches, i);
            
            if (!InBitTools::__getbit(connections->branches, i)) do {
               i--;
               depth--;
            } while (InBitTools::__getbit(connections->branches, i) && i);

            else do {
               i--;
               InBitTools::__flipbit(character_buffer[head_pointer->character].code.branches, i);
            } while (InBitTools::__getbit(connections->branches, i) && i);

            // Print the Huffman tree to the standart output. Will be made optional
            {
               // Store the possible ways that a tree can branch
               static const char branches[] = {
                  '\xB3', '\x20', // "│ ", the top cell connects, but not the sides
                  '\xC0', '\xC4', // "┬─", the top cell doesn't connect, but both sides do
                  '\xC2', '\xC4', // "└─", the top cell and the right side connect
                  '\x20', '\x20'  // "  ", no connections
               };

               i = 0;
               while (i < character_buffer[head_pointer->character].code.length) {
                  uchar index = 0;
                  if (InBitTools::__getbit(connections->branches, i)) index |= 4;
                  if (InBitTools::__getbit(character_buffer[head_pointer->character].code.branches, i)) index |= 2;

                  std::cout.write(&branches[index], 2);
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
                     std::cout << 'x' << std::hex << std::setw(2)
                     << std::setfill('0') << (short)(head_pointer->character);
               }
               else if (head_pointer->character == 0x20)
                  // {20}
                  std::cout << "' '";
               else {
                  // (20, 7F) = [21, 7E]
                  std::cout.put(head_pointer->character);
               }

               std::cout << ": " << std::dec << head_pointer->frequency << "\n";
            }

            entropy += character_buffer[head_pointer->character].code.length * head_pointer->frequency; // ∑L(x)f(x)
            connections = &character_buffer[head_pointer->character].code;
            // #endif
         }

         // Pops the stack, i.e., the head_pointer
         TreeNode* tmp = head_pointer->next_node;
         delete head_pointer;
         head_pointer = tmp;
      }

      std::cout << "Entropy: " << (float)entropy / file_length << '\n';
      std::cout << "Total character count: " << file_length << '\n';
   }

   // Create the new file
   /* {
      output_file.put(0x00);

      output_file.write((char*)header, header_size);
      delete[] header;

      uchar*& buffer = header;
      buffer = new uchar[entropy];
      for (uint64 i = 0; i < entropy; i++) {
         buffer[i] = 0xff;
      }

      uint64 byte_cursor = 0;
      uchar bit_cursor = 7;

      uchar ch;
      while (input_file.get((char&)ch)) {

      }

      output_file.write((char*)buffer, entropy);
   } */

   delete[] placeholder.branches;
   return true;
}