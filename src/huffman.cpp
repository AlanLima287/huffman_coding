#pragma once

#include "huffman.h"

bool HuffmanCoding::encode(const char* input_filename, const char* output_filename) {
   State state;

   state.hufifile.file = fopen(input_filename, "r+");
   if (!state.hufifile.file) return false;

   setvbuf(state.hufifile.file, nullptr, _IONBF, 0);

   state.hufifile.buffer = new (std::nothrow) byte[PAGE_SIZE + 1];
   if (!state.hufifile.buffer) {
      fclose(state.hufifile.file);
      return false;
   }

   state.hufifile.size = fread(state.hufifile.buffer, sizeof(byte), PAGE_SIZE, state.hufifile.file);
   state.hufifile.length = state.hufifile.size;
   state.hufifile.cursor = 0;

   find_frequencies(state);
   build_tree(state);

   delete[] state.hufifile.buffer;
   fclose(state.hufifile.file);

   return true;
}

bool HuffmanCoding::find_frequencies(State& state) {
   uint64_t character_buffer[256] = {};
   byte ch;

   uint16_t unique_characters = 0;

   // Reads the file character-wise incrementing for the corresponding
   // character frequency and file length
   while (getchar(state.hufifile, ch)) {
      if (!character_buffer[ch])
         unique_characters++;

      character_buffer[ch]++;
   }

   if (unique_characters <= 1) {
      return false; // Temporary
   }

   // A full tree with n leaf-nodes will always have 2n - 1 nodes
   state.base = new (std::nothrow) Node[2 * unique_characters - 1];
   if (!state.base) return false;

   state.head = undefined;
   state.index = 0;

   // ch will now be used as an index
   ch = 0xff;

   while (true) {
      // Check whether the frequency is non-zero
      if (character_buffer[ch]) {

         // Set stuff up
         state.base[state.index].frequency = character_buffer[ch];
         state.base[state.index].character = ch;
         state.base[state.index].height = 0;

         // Initialize both children to undefined
         state.base[state.index].left_child = undefined;
         state.base[state.index].rght_child = undefined;

         intnode_t* node = &state.head;

         while (true) {
            if (*node == undefined) break;
            if (state.base[*node].frequency >= character_buffer[ch]) break;

            node = &state.base[*node].next_node;
         }

         /* Insert the new node [N] somewhere at the queue
         *
         * [h] → ... → [n] → [m] → ... => [h] → ... → [n] ─/→ [m] = [h] → ... → [n] → [N] → [m] → ...
         *             [N]                             └→ [N]  ↑
         *                                                 └───┘
         */
         state.base[state.index].next_node = *node;
         *node = state.index;

         state.index++;
      }

   continuing:
      // Stops when ch is 0, if not 0, decrements it;
      if (!ch) break;
      ch--;
   }

   return true;
}

void HuffmanCoding::build_tree(State& state) {

   intnode_t* node = &state.head;

   // Yes, the loop will execute state.index - 1 times
   for (uint64_t i = state.index; i > 1; i--) {

      print_queue(state);

      intnode_t next_node = state.base[state.head].next_node;
      state.base[state.index].frequency = state.base[state.head].frequency + state.base[next_node].frequency;

      // Guarantees that the right child will never be higher than the left child
      if (state.base[state.head].height > state.base[next_node].height) {
         
         state.base[state.index].left_child = next_node;
         state.base[state.index].rght_child = state.head;

         state.base[state.index].height = 1 + state.base[state.head].height;
      
      } else {
         state.base[state.index].left_child = state.head;
         state.base[state.index].rght_child = next_node;

         state.base[state.index].height = 1 + state.base[next_node].height;
      }
      
      while (true) {

         if (*node == undefined) break;
         if (state.base[*node].frequency > state.base[state.index].frequency) break;
         if (state.base[*node].frequency == state.base[state.index].frequency) {
            if (state.base[*node].height >= state.base[state.index].height) break;
         }

         node = &state.base[*node].next_node;
      }

      /* Insert the new node [N] somewhere at the queue
      *
      * [h] → ... → [n] → [m] → ... => [h] → ... → [n] ─/→ [m] = [h] → ... → [n] → [N] → [m] → ...
      *             [N]                             └→ [N]  ↑
      *                                                 └───┘
      */
      state.base[state.index].next_node = *node;
      *node = state.index;

      state.head = state.base[state.base[state.head].next_node].next_node;

      state.base[state.base[state.index].left_child].next_node = undefined;
      state.base[state.base[state.index].rght_child].next_node = undefined;

      state.index++;
   }

   print_queue(state);
}

inline bool HuffmanCoding::getchar(HUFIFile& hufifile, byte& ch) {
   if (hufifile.cursor >= hufifile.size) {

      if (feof(hufifile.file)) return false;

      hufifile.size = fread(hufifile.buffer, sizeof(byte), PAGE_SIZE, hufifile.file);
      if (!hufifile.size) return false;

      hufifile.length += hufifile.size;
      hufifile.cursor = 0;
   }

   ch = hufifile.buffer[hufifile.cursor++];
   return true;
}

/*
bool HuffmanCoding::encode(const char* input_filename, const char* output_filename) {

   // Loads the input file
   std::fstream input_file(input_filename, std::ios::in | std::ios::binary);
   if (!input_file) return false;

   // Loads the output file
   std::fstream output_file(output_filename, std::ios::out | std::ios::binary);
   if (!output_file) return true;

   // Holds the count/code for every ASCII character
   Buffer character_buffer[256] = {};

   // character_buffer's code variables will point to objects on this buffer
   byte* holder = nullptr;
   HUFFileHeader flags = {};

   uint64_t file_length = 0; // Stores the length of the file
   uint64_t entropy = 0; // Stores the (scaled by file_lenght) Shannon entropy of the Huffman tree
   byte length = 0; // Stores the height of the tree in bytes

   Node* head_pointer = nullptr; // It is the head of the priority queue

   unsigned short unique_characters = 0;
   uint64_t header_size;

   {
      byte ch;

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
            head_pointer = new Node{ 0, ch, character_buffer[ch].count, nullptr, nullptr, head_pointer };
            continue;
         }

         for (Node* n = head_pointer; n; n = n->next_node) {
            if (!n->next_node || n->next_node->frequency >= character_buffer[ch].count) {
               // Creates a new node [N] an inserts it somewhere else at the queue
               //
               // [h] → ... → [n] → [m] → ... => [h] → ... → [n] ─/→ [m] = [h] → ... → [n] → [N] → [m] → ...
               //             [N]                             └→ [N]  ↑
               //                                                 └───┘
               n->next_node = new Node{ 0, ch, character_buffer[ch].count, nullptr, nullptr, n->next_node };
               break;
            }
         }

         // Stops when ch is 0, if not 0, decreases it;
      } while (ch--);

      flags.true_tree = unique_characters > 1;
      header_size = (uint64_t)unique_characters << 3;
   }

   // Building the Huffman tree
   {
      while (head_pointer && head_pointer->next_node) {

         byte height = 1 + (head_pointer->height >= head_pointer->next_node->height ? head_pointer->height : head_pointer->next_node->height);
         //byte height = 1 + head_pointer->height + head_pointer->next_node->height;

         uint64_t frequency = head_pointer->frequency + head_pointer->next_node->frequency;

         // Garantees that the right child will never be higher than the left child
         Node* l_child;
         Node* r_child;
         if (head_pointer->height <= head_pointer->next_node->height) {
            l_child = head_pointer;
            r_child = head_pointer->next_node;
         }
         else {
            l_child = head_pointer->next_node;
            r_child = head_pointer;
         }

         for (Node* n = head_pointer->next_node; n; n = n->next_node) {
            if (!n->next_node || frequency < n->next_node->frequency || (frequency == n->next_node->frequency && height < n->next_node->height)) {

               n->next_node = new Node{
                  height, byte('0' + ((!!l_child->height << 1) | !!r_child->height)), frequency, l_child, r_child, n->next_node
               };

               break;
            }
         }

         header_size++;
         if (l_child->height)
            header_size++;

         head_pointer = head_pointer->next_node->next_node;
         l_child->next_node = nullptr;
         r_child->next_node = nullptr;
      }

      length = (head_pointer->height + 7) >> 3;
   }

   // Enconding the tree as a sequence of characters
   {
      header_size = sizeof(file_length) + ((header_size + 7) >> 3); // From bits to bytes plus the size of the header
      byte* header = new byte[sizeof(file_length) + header_size]; // Points to the header, i.e., the encoded tree

      uint64_t i = 0;

      // *(uint64_t*)header = file_length;
      // Does guarantee the Little Endian ordering, unlike above
      for (; i < sizeof(file_length); i++) {
         header[i] = byte(file_length >> (i << 3));
         //flags.size_order++;
      }

      // Initialize the whole header to NULL bytes
      // Necessary for future bitwise or operatios
      for (; i < header_size; i++)
         header[i] = 0;

      uint64_t byte_cursor = sizeof(file_length); // Index of the writing head of the header
      byte bit_cursor = 7; // Index in-byte of the writing head of the header

      // byte height = head_pointer->height; // Stores the absolute height of the tree
      byte depth = 0; // Stores the relative depth of the tree

      holder = new byte[(unique_characters + 1) * length];
      CodeWord placeholder = { 0, holder };

      for (byte i = 0; i < length; i++) {
         placeholder.branches[i] = 0;
      }

      CodeWord* connections = &placeholder;

      // The head_pointer will now be used as a stack top
      while (head_pointer) {

         // for (Node* n = head_pointer; n; n = n->next_node) {
         //    std::cout.put(n->character);
         //    std::cout.put('-');
         // }
         // std::cout.put('\n');

         if (head_pointer->height) {
            if (head_pointer->left_child->height) {
               // If the current node has left child as a leaf, put 1
               header[byte_cursor] |= 1 << bit_cursor;

               if (!bit_cursor) { bit_cursor = 8; byte_cursor++; }
               bit_cursor--;

               if (head_pointer->rght_child->height)
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
            head_pointer->rght_child->next_node = head_pointer->next_node;
            head_pointer->left_child->next_node = head_pointer->rght_child;
            head_pointer->next_node = head_pointer->left_child;

            depth++; // If a tree has branched (has children), the depth is incremented
         }
         else {
            header[byte_cursor++] |= head_pointer->character >> (7 - bit_cursor); // First part of the character
            header[byte_cursor] |= head_pointer->character << (1 + bit_cursor); // Second part of the character

            character_buffer[head_pointer->character].code.branches = connections->branches + length;
            character_buffer[head_pointer->character].code.length = depth;

            byte i; // Iterator variable
            for (i = 0; i < length; i++)
               character_buffer[head_pointer->character].code.branches[i] = connections->branches[i];

            i = depth - 1;

            // Well, I did explain it, after all :>
            //
            // Enjoy a Turing Machine!!!
            // States:            {q₀, q₁, HALT};
            // Symbols:           {0, 1, _};
            // Blank symbol:      _;
            // Initial state:     q₀;
            // Behavior function: δ.
            //
            //            Instruction table for δ
            // Configuration       Behavior
            // State     Symbol    Symbol    Movement  State
            // q₀        0         1         R         q₁
            // q₀        1         0         R         q₀
            // q₁        0         0         R         q₁
            // q₁        1         1         R         q₁
            // q₁        _         _                   HALT
            //
            // For any branch code member of 1ⁿ{0,1}*, n is decremented from the depth,
            // means unbranching (exiting a branch)

            InBitTools::__flipbit(character_buffer[head_pointer->character].code.branches, i);
            if (i) {
               if (InBitTools::__getbit(connections->branches, i)) do {
                  i--;
                  depth--;
               } while (!InBitTools::__getbit(connections->branches, i) && i);

               else do {
                  i--;
                  InBitTools::__flipbit(character_buffer[head_pointer->character].code.branches, i);
               } while (!InBitTools::__getbit(connections->branches, i) && i);
            }

            // Print the Huffman tree to the standart output. Will be made optional
            {
               // Store the possible ways that a tree can branch
               static const char branches[] = {
                  '\x20', '\x20', // "  ", no connections
                  '\xC2', '\xC4', // "└─", the top cell and the right side connect
                  '\xC0', '\xC4', // "┬─", the top cell doesn't connect, but both sides do
                  '\xB3', '\x20', // "│ ", the top cell connects, but not the sides
               };

               for (i = 0; i < character_buffer[head_pointer->character].code.length; i++) {
                  byte index = 0;
                  if (InBitTools::__getbit(connections->branches, i)) index |= 4;
                  if (InBitTools::__getbit(character_buffer[head_pointer->character].code.branches, i)) index |= 2;

                  std::cout.write(&branches[index], 2);
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
                  num = 20: ' '.
               */
/*
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

               std::cout << ": " << std::dec << (short)head_pointer->frequency << ' '
                  << (short)character_buffer[head_pointer->character].code.length << ' ';

               // for (byte i = 0; i < length; i++) {
               //    print_bits(~character_buffer[head_pointer->character].code.branches[i], 8);
               //    std::cout.put(' ');
               // }

               for (byte i = 0; i < character_buffer[head_pointer->character].code.length; i++)
                  std::cout.put('1' - InBitTools::__getbit(character_buffer[head_pointer->character].code.branches, i));

               std::cout.put('\n');
            }

            entropy += character_buffer[head_pointer->character].code.length * head_pointer->frequency; // ∑L(x)f(x)
            connections = &character_buffer[head_pointer->character].code;
         }

         // Pops the stack, i.e., the head_pointer
         Node* tmp = head_pointer->next_node;
         delete head_pointer;
         head_pointer = tmp;
      }

      std::cout << "Entropy: " << (float)entropy / file_length << '\n';
      std::cout << "Total character count: " << file_length << '\n';

      output_file.put(flags.state);
      output_file.write((char*)header, header_size);
      delete[] header;
   }

   // Encoding the file
   {
      entropy = (entropy + 7) >> 3; // From bits to bytes
      byte* buffer = new byte[entropy]; // Holds the encoded file

      // Initialize the whole buffer to FULL bytes
      // Necessary for future bitwise and operations
      for (uint64_t i = 0; i < entropy; i++) {
         buffer[i] = 0xff;
      }

      uint64_t bbit_cursor = 0;

      input_file.close();
      input_file.open(input_filename, std::ios::in | std::ios::binary);

      byte ch;
      while (input_file.get((char&)ch)) {

         byte code_length = (character_buffer[ch].code.length - 1) >> 3;
         byte i;

         for (i = 0; i < code_length; i++) {
            buffer[bbit_cursor >> 3] &= ~(character_buffer[ch].code.branches[i] >> (bbit_cursor & 0x7));
            bbit_cursor += 8;

            buffer[bbit_cursor >> 3] &= ~(character_buffer[ch].code.branches[i] << (8 - (bbit_cursor & 0x7)));
         }

         buffer[bbit_cursor >> 3] &= ~(character_buffer[ch].code.branches[i] >> (bbit_cursor & 0x7));

         if (((character_buffer[ch].code.length - 1) & 0x7) + (bbit_cursor & 0x7) > 7)
            buffer[(bbit_cursor >> 3) + 1] &= ~(character_buffer[ch].code.branches[i] << (8 - (bbit_cursor & 0x7)));

         bbit_cursor += ((character_buffer[ch].code.length - 1) & 0x7) + 1;
      }

      output_file.write((char*)buffer, entropy);
      delete[] buffer;
   }

   output_file.close();
   input_file.close();

   delete[] holder;
   return true;
}
*/