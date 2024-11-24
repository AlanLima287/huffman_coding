#pragma once

#include "huffman.h"

bool HuffmanCoding::open_files(State& state, const char* input_filename, const char* output_filename) {

   using HUFFile::exit_t, HUFFile::mode_t;

   exit_t response;

   response = HUFFile::open(state.ifile, input_filename, mode_t::read);
   if (response != exit_t::success) goto error;

   response = HUFFile::open(state.ofile, output_filename, mode_t::overwrite /* temp, remember to put it back to just write */);

   if (response == exit_t::file_already_exists) while (true) {
      std::cout << "O arquivo " << output_filename << " já existe, deseja sobreescreve-lo ([s]im ou [n]ão)? ";

      char answer = std::cin.get();
      std::cin.ignore(64, '\n');

      if (answer == 'n') return false;
      if (answer != 's') continue;

      response = HUFFile::open(state.ofile, output_filename, mode_t::overwrite);
   }

   if (response == exit_t::success) return true;

error:
   switch (response) {
      case exit_t::buffering_error:
         std::cout.write("Algum erro ocorreu ao preparar o buffer para o arquivo!\n", 56);
         break;

      case exit_t::bad_allocation:
         std::cout.write("\e[37;41;1mErro Fatal:\e[0;31m Má Alocação\e[m", 46);
         break;
   }

   return false;
}

bool HuffmanCoding::encode(const char* input_filename, const char* output_filename) {

   using HUFFile::exit_t;

   State state;
   byte last_ch;

   if (!open_files(state, input_filename, output_filename)) {
      return false;
   }

   state.flags.literal = 0;

   // Only useful for the 1 case of the switch below
   last_ch = encoding::find_frequencies(state);

   switch (state.unique_chars) {

      // The encoded empty file will be an empty file
      case 0: break;

      // If there is only one kind of character, it cannot
      // be encoded under the Huffman coding
      case 1: {
         state.flags.header.true_tree = 0;

         HUFFile::putbits(state.ofile, &state.flags.literal, 8);
         HUFFile::putbits(state.ofile, &last_ch, 8);

         for (uint64_t i = 0; state.size >> i; i += 8) {
            byte size = byte((state.size >> i) & 0xFF);
            HUFFile::putbits(state.ofile, &size, 8);
         }

         HUFFile::flushbits(state.ofile);
      } break;

      default: {
         state.flags.header.true_tree = 1;

         if (!encoding::build_queue(state)) goto bad_allocation;
         encoding::build_tree(state);

         if (!encoding::build_header(state)) goto bad_allocation;

         // The tree is no longer needed nor valid
         delete[] state.base;

         HUFFile::rewind(state.ifile);

         byte ch;
         while (HUFFile::getchar(state.ifile, ch)) {
            HUFFile::putbits(state.ofile,
               state.character_buffer[ch].code->branch,
               state.character_buffer[ch].code->length);
         }

         HUFFile::flushbits(state.ofile);

         HUFFile::rewind(state.ofile);
         HUFFile::putbits(state.ofile, &state.flags.literal, 8);

         HUFFile::flushbits(state.ofile);

         delete[] state.connections;
         delete[] state.branches;

      } break;
   }

   HUFFile::close(state.ifile);
   HUFFile::close(state.ofile);
   return true;

bad_allocation:
   std::cout.write("\e[37;41;1mErro Fatal:\e[0;31m Má Alocação\e[m", 46);
   return false;
}

bool HuffmanCoding::decode(const char* input_filename, const char* output_filename) {

   using HUFFile::exit_t;

   State state;

   if (!open_files(state, input_filename, output_filename)) {
      return false;
   }

   if (!HUFFile::getbyte(state.ifile, state.flags.literal)) {
      goto terminate; // It was a empty file
   }

   switch (state.flags.header.true_tree) {

      // The file had only one kind of character
      case 0: {
         byte ch, character;
         uint64_t count = 0;

         // Get that one character
         HUFFile::getbyte(state.ifile, character);

         // Get the number of repetitions
         for (uint8_t i = 0; HUFFile::getbyte(state.ifile, ch); i += 8) {
            count |= ch << i;
         }

         // Put the character the appropriate number of times
         for (uint64_t i = 0; i < count; i++) {
            HUFFile::putchar(state.ofile, character);
         }

         HUFFile::flush(state.ofile);
      } break;

      // The file was encoded under the Huffman coding
      case 1: {
         decoding::build_tree_branch(state, &state.head);
         print_inline_tree(state.head);

         while (true) {
            byte ch;
            if (!decoding::get_character(state, ch)) {
               goto terminate;
            }

            HUFFile::putchar(state.ofile, ch);

            if (state.ifile.size > 8 + state.ifile.cursor) continue;
            if ((state.ifile.cursor & 0b111) < state.flags.header.last_byte) continue;
               
            HUFFile::flush(state.ofile);
            goto terminate;
         }

      } break;
   }

terminate:
   HUFFile::close(state.ifile);
   HUFFile::close(state.ofile);
   return true;
}

byte HuffmanCoding::encoding::find_frequencies(State& state) {

   memset(state.character_buffer, 0, sizeof(state.character_buffer));
   state.unique_chars = 0;
   state.size = 0;

   byte ch = 0;

   // Reads the file character-wise incrementing for the corresponding
   // character frequency and file length
   while (getchar(state.ifile, ch)) {
      if (!state.character_buffer[ch].count)
         state.unique_chars++;

      state.character_buffer[ch].count++;
      state.size++;
   }

   return ch;
}

bool HuffmanCoding::encoding::build_queue(State& state) {

   // A full tree with n leaf-nodes will always have 2n - 1 nodes
   state.base = new (std::nothrow) Node[2 * state.unique_chars - 1];
   if (!state.base) return false;

   Node* index = state.base;
   state.head = nullptr;

   byte ch = 0xff;

   while (true) {
      // Check whether the frequency is non-zero
      if (state.character_buffer[ch].count) {

         // Set stuff up
         index->frequency = state.character_buffer[ch].count;
         index->character = ch;
         index->height = 0;

         // Initialize both children to a nullptr
         index->child[0] = nullptr;
         index->child[1] = nullptr;

         Node** node = &state.head;

         while (true) {
            if (*node == nullptr) break;
            if ((*node)->frequency >= index->frequency) break;

            node = &(*node)->next_node;
         }

         /* Insert the new node [N] somewhere at the queue
         *
         * [h] → ... → [n] → [m] → ... => [h] → ... → [n] ─/→ [m] = [h] → ... → [n] → [N] → [m] → ...
         *             [N]                             └→ [N]  ↑
         *                                                 └───┘
         */
         index->next_node = *node;
         *node = index;

         index++;
      }

      // Stops when ch is 0, if not 0, decrements it
      if (!ch) break;
      ch--;
   }

   return true;
}

void HuffmanCoding::encoding::build_tree(State& state) {

   Node** node = &state.head;
   Node* index = state.base + state.unique_chars;

   // Yes, the loop will execute state.unique_chars - 1 times,
   // this was intentional
   for (uint64_t i = state.unique_chars; i > 1; i--) {

      // print_queue(state);

      Node* next_node = state.head->next_node;
      index->frequency = state.head->frequency + next_node->frequency;

      // Guarantees that the right child will never be higher than the left child
      if (state.head->height > next_node->height) {

         index->child[0] = next_node;
         index->child[1] = state.head;

         index->height = 1 + state.head->height;

      } else {
         index->child[0] = state.head;
         index->child[1] = next_node;

         index->height = 1 + next_node->height;
      }

      /* +-------------+-----------+-----------+
      *  | Two leaves  | One leaf  | No leaves |
      *  +-------------+-----------+-----------+
      *  |     [ ]     |    [ ]    |    [ ]    |
      *  |     / \     |    / \    |    / \    |
      *  |   [A] [B]   |  [A] [ ]  |  [ ] [ ]  |
      *  +-------------+-----------+-----------+
      *
      * [A] and [B] are caracters, that is, they are leaves,
      * [ ] represents a non-leaf node.
      */
      if (!index->child[1]->height) {
         index->character = two_leaves;
      } else {
         if (!index->child[0]->height) {
            index->character = one_leaf;
         } else {
            index->character = no_leaves;
         }
      }

      while (*node) {
         if ((*node)->frequency > index->frequency) break;
         if ((*node)->frequency == index->frequency) {
            if ((*node)->height >= index->height) break;
         }

         node = &(*node)->next_node;
      }

      /* Insert the new node [N] somewhere at the queue
      *
      * [h] → ... → [n] → [m] → ... => [h] → ... → [n] ─/→ [m] = [h] → ... → [n] → [N] → [m] → ...
      *             [N]                             └→ [N]  ↑
      *                                                 └───┘
      */
      index->next_node = *node;
      *node = index;

      state.head = state.head->next_node->next_node;

      index->child[0]->next_node = nullptr;
      index->child[1]->next_node = nullptr;

      index++;
   }

   // print_queue(state);
}

bool HuffmanCoding::encoding::build_header(State& state) {

   // Placeholder for the flags that will come later
   // We must know the entropy first
   state.ofile.cursor = 8;

   uint64_t entropy = 0;
   uint8_t length = BitTools::size(state.head->height);
   uint8_t depth = 0; // Stores the relative depth of the tree

   state.connections = new (std::nothrow) CodeWord[state.unique_chars];
   if (!state.connections) return false;

   state.branches = new (std::nothrow) byte[state.unique_chars * length];
   if (!state.branches) {
      delete[] state.branches;
      return false;
   }

   CodeWord* connections = state.connections;
   byte* branches = state.branches;

   for (int16_t i = length - 1; i >= 0; i--)
      branches[i] = 0xFF;

   print_inline_tree(state.head);
   std::cout.put('\n');

   // The head will now be used as a stack top
   while (true) {

      if (state.head->height) {

         byte branch_style, length;
         switch (state.head->character) {
            case no_leaves:  branch_style = 0b11; length = 2; break;
            case one_leaf:   branch_style = 0b01; length = 2; break;
            case two_leaves: branch_style = 0b00; length = 1; break;
         }

         HUFFile::putbits(state.ofile, &branch_style, length);

         /* Pointer jugling, take your time (Setup for popping the stack later)
         *                                     ┌───┐
         *     [h] → [n]    ┌──── [h]   [n]   [h]  ↓
         *   ┌──┴──┐     => │   ┌──┴──┐  ↑  =  │  [0] → [1] → [n]
         *  [0]   [1]       └→ [0] → [1] ┘     └───┴─────┘
         */
         state.head->child[1]->next_node = state.head->next_node;
         state.head->child[0]->next_node = state.head->child[1];
         state.head->next_node = state.head->child[0];

         depth++; // If a tree has branched (has children), the depth is incremented

      } else {

         byte ch = state.head->character;
         HUFFile::putbits(state.ofile, &ch, 8);

         entropy += depth * state.head->frequency; // ∑L(x)f(x)

         state.character_buffer[ch].code = connections;
         connections->branch = branches;
         connections->length = depth;

         /* Well, I did explain it, after all :>
         *
         *  Enjoy a Turing Machine!!!
         *  States:            {q₀, HALT};
         *  Symbols:           {0, 1, _};
         *  Blank symbol:      _;
         *  Initial state:     q₀;
         *  Behavior function: δ.
         *
         *             Instruction table for δ
         *  Configuration       Behavior
         *  State     Symbol    Symbol    Movement  State
         *  q₀        1         0         R         q₀
         *  q₀        0         1         R         HALT
         *  q₀        _         _                   HALT
         *
         *  For any branch code member of 1ⁿ{0{0,1}*,_}, n is decremented from the depth,
         *  means unbranching (exiting a branch)
         */
         byte i = depth - 1;

         BitTools::flipbit(connections->branch, i);

         if (i) {
            if (BitTools::getbit(connections->branch, i)) do {
               i--;
               depth--;
            } while (BitTools::getbit(connections->branch, i) && i);

            else do i--;
            while (BitTools::flipbit(connections->branch, i) && i);
         }

         BitTools::print(connections->branch, connections->length);
         std::cout.write(": ", 2);
         print_character(ch);
         std::cout.put('\n');

         if (state.head->next_node == nullptr) break;


         // The current code will be used as a base for the next one
         branches += length;

         for (byte i = 0; i < length; i++)
            branches[i] = connections->branch[i];

         connections++;
      }

      // Pops the stack
      state.head = state.head->next_node;
   }

   std::cout << "Entropy: " << (float)entropy / state.size << '\n';
   std::cout << "Total character count: " << state.size << '\n';

   // This will give back the position of the last true bit in the last byte of the encoded stream,
   // state.ofile.cursor, as of now, stores the size of the header (flags byte plus the encoded tree)
   // modded by a multiple of 8 and entropy is the amount of bits in the whole encoded stream
   state.flags.header.last_byte = (entropy + state.ofile.cursor) & 0b111;

   return true;
}

bool HuffmanCoding::decoding::build_tree_branch(State& state, Node** node) {
   *node = new (std::nothrow) Node{ 0, nullptr, nullptr, nullptr, 0, 1 };
   if (!*node) return false;

   byte bit;
   HUFFile::getbit(state.ifile, bit);

   if (!bit) {

      build_tree_leaf(state, &(*node)->child[0]);
      build_tree_leaf(state, &(*node)->child[1]);

   } else {

      HUFFile::getbit(state.ifile, bit);

      if (!bit) build_tree_leaf(state, &(*node)->child[0]);
      else build_tree_branch(state, &(*node)->child[0]);

      build_tree_branch(state, &(*node)->child[1]);
   }

   return true;
}

bool HuffmanCoding::decoding::build_tree_leaf(State& state, Node** node) {
   byte ch;
   HUFFile::getbyte(state.ifile, ch);

   *node = new (std::nothrow) Node{ 0, nullptr, nullptr, nullptr, ch, 0 };
   return *node != nullptr;
}

bool HuffmanCoding::decoding::get_character(State& state, byte& ch) {

   Node* node = state.head;
   byte bit;

   do {
      HUFFile::getbit(state.ifile, bit);
      node = node->child[bit];
   } while (node->height);

   ch = node->character;
   return true;
}