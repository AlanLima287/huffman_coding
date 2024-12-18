#pragma once

#include "huffman.h"

bool HuffmanCoding::open_files(HUFFile::File& ifile, HUFFile::File& ofile, const char* input_filename, const char* output_filename) {

   using HUFFile::exit_t, HUFFile::mode_t;

   exit_t response;

   response = HUFFile::open(ifile, input_filename, mode_t::read);
   if (response != exit_t::success) goto error;

   response = HUFFile::open(ofile, output_filename, mode_t::write);

   if (response == exit_t::file_already_exists) while (true) {
      std::cout << "O arquivo " << output_filename << " já existe, deseja sobreescreve-lo ([s]im ou [n]ão)? ";

      char answer = std::cin.get();
      std::cin.ignore(64, '\n');

      if (answer == 'n') return false;
      if (answer != 's') continue;

      response = HUFFile::open(ofile, output_filename, mode_t::overwrite);
      break;
   }

   if (response == exit_t::success) return true;

error:
   switch (response) {
      case exit_t::buffering_error:
         std::cout.write("Algum erro ocorreu ao preparar o buffer para o arquivo!\n", 56);
         break;

      case exit_t::bad_allocation:
         std::cout.write(BAD_ALLOCATION, sizeof(BAD_ALLOCATION) - 1);
         break;

      default:
         break;
   }

   return false;
}

bool HuffmanCoding::encode(const char* input_filename, const char* output_filename, bool show_tree) {

   using HUFFile::exit_t;

   encoding::State state;
   byte last_ch;

   if (!open_files(state.ifile, state.ofile, input_filename, output_filename)) {
      return true;
   }

   state.flags.literal = 0;

   // last_ch is only useful for the 1 case of the switch below
   std::cout.write("Lendo o arquivo...\r", 19);
   last_ch = encoding::find_frequencies(state);
   esc::clear_line();

   switch (state.unique_chars) {

      // The encoded empty file will be an empty file
      case 0: break;

      // If there is only one kind of character, it cannot
      // be encoded under the Huffman coding
      case 1: {
         state.flags.header.true_tree = 0;

         if (show_tree) {
            print_character(last_ch);
            std::cout.put('\n');
         }

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

         /* for the user interface */ {
            if (show_tree)
               application::print_tree(state.head);
         }

         if (!encoding::build_header(state)) goto bad_allocation;

         // The tree is no longer needed nor valid
         delete[] state.base;

         HUFFile::rewind(state.ifile);

         // Stuff for the loading bar
         uint16_t width, progress;
         uint64_t chunck;

         /* for the user interface */ {
            get_terminal_dimensions(width, progress);
            chunck = state.size / width;

            std::cout.put('[');
            esc::move_to(width);
            std::cout.put(']');

            esc::move_to(1);
         }

         byte ch;
         for (uint64_t i = 0, j = 0; HUFFile::getchar(state.ifile, ch); i++) {
            HUFFile::putbits(state.ofile,
               state.character_buffer[ch].code->branch,
               state.character_buffer[ch].code->length);

            if (i >= chunck) { // for the user interface
               j += i;
               i = 0;

               uint16_t new_progress = j * width / state.size;
               for (; progress < new_progress; progress++)
                  std::cout.put('#');
            }
         }

         esc::clear_line();
         std::cout.put('\r');

         state.encoded_size /= 8; // bits to bytes

         std::cout << "Tamanho original: " << state.size << " bytes\n";
         std::cout << "Tamanho após compressão: " << state.encoded_size << " bytes\n";
         std::cout << "Taxa de compressão: " << (1.0 - (double)state.encoded_size / state.size) * 100.0 << "%\n";

         HUFFile::flushbits(state.ofile);

         // Putting the flag now
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
   std::cout.write(BAD_ALLOCATION, sizeof(BAD_ALLOCATION) - 1);
   return false;
}

bool HuffmanCoding::decode(const char* input_filename, const char* output_filename, bool show_tree) {

   using HUFFile::exit_t;

   decoding::State state;

   if (!open_files(state.ifile, state.ofile, input_filename, output_filename)) {
      return true;
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
         if (!HUFFile::getbyte(state.ifile, character)) break;

         if (show_tree) {
            print_character(character);
            std::cout.put('\n');
         }

         // Get the number of repetitions
         for (uint8_t i = 0; HUFFile::getbyte(state.ifile, ch); i += 8) {
            count |= ch << i;
         }

         // Put the character the appropriate number of times
         for (uint64_t i = 0; i < count; i++)
            HUFFile::putchar(state.ofile, character);

         HUFFile::flush(state.ofile);
      } break;

      // The file was encoded under the Huffman coding
      case 1: {

         // The -1 reffers to the flag byte already computed
         state.size = (HUFFile::file_size(state.ifile) - 1) << 3;
         if (state.flags.header.last_byte)
            state.size -= 8 - state.flags.header.last_byte;

         if (!decoding::build_tree_branch(state, &state.head))
            goto bad_allocation;

         if (show_tree)
            application::print_tree(state.head);

         // Stuff for the loading bar
         uint16_t width, progress;
         uint64_t chunck;

         /* for the user interface */ {
            get_terminal_dimensions(width, progress);
            chunck = state.size / width;

            std::cout.put('[');
            esc::move_to(width);
            std::cout.put(']');

            esc::move_to(1);
         }

         for (uint64_t i = 0, j = 0; i < state.size;) {
            Node* node = state.head;
            byte bit;

            do {
               if (!HUFFile::getbit(state.ifile, bit)) goto finish;
               node = node->child[bit];
               i++;
            } while (node->height);

            HUFFile::putchar(state.ofile, node->character);

            j += i;
            if (j >= chunck) { // for the user interface
               j %= chunck;

               uint16_t new_progress = i * width / state.size;
               for (; progress < new_progress; progress++)
                  std::cout.put('#');
            }
         }

      finish:
         esc::clear_line();
         std::cout.put('\r');

         HUFFile::flush(state.ofile);
         decoding::destroy_tree(state.head);

      } break;
   }

terminate:
   HUFFile::close(state.ifile);
   HUFFile::close(state.ofile);
   return true;

bad_allocation:
   std::cout.write(BAD_ALLOCATION, sizeof(BAD_ALLOCATION) - 1);
   return false;
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
         index->next_node = nullptr;
         index->character = ch;
         index->height = 0;

         // Initialize both children to a nullptr
         index->child[0] = nullptr;
         index->child[1] = nullptr;

         Node** node = &state.head;

         while (*node && (*node)->frequency < index->frequency)
            node = &(*node)->next_node;

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

   Node* index = state.base + state.unique_chars;
   state.encoded_size = state.unique_chars * 8 + 8;

   // Yes, the loop will execute state.unique_chars - 1 times, this was intentional
   for (uint64_t i = 1; i < state.unique_chars; i++) {

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
         state.encoded_size += 1;
      } else {
         index->character = index->child[0]->height ? no_leaves : one_leaf;
         state.encoded_size += 2;
      }

      Node** node = &next_node->next_node;
      while (*node && (*node)->frequency < index->frequency)
         node = &(*node)->next_node;

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
}

bool HuffmanCoding::encoding::build_header(State& state) {

   // Placeholder for the flags that will come later
   // We must know the entropy first
   state.ofile.cursor = 8;

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

   // The head will now be used as a stack top
   while (true) {

      if (state.head->height) {

         byte branch_style = 0, length = 0;
         switch (state.head->character) {
            case no_leaves:  branch_style = 0b11; length = 2; break;
            case one_leaf:   branch_style = 0b01; length = 2; break;
            case two_leaves: branch_style = 0b0;  length = 1; break;
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

         state.encoded_size += depth * state.head->frequency; // ∑L(x)f(x)

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
         bool bit = BitTools::flipbit(connections->branch, i);

         if (i) {
            if (bit) do { i--; } while (BitTools::flipbit(connections->branch, i) && i);
            else do { i--; depth--; } while (BitTools::getbit(connections->branch, i) && i);
         }

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

   // This will give back the position of the last true bit in the last byte of the encoded stream
   state.flags.header.last_byte = state.encoded_size & 0b111;

   return true;
}

bool HuffmanCoding::decoding::build_tree_branch(State& state, Node** node) {
   *node = new (std::nothrow) Node{ 0, nullptr, nullptr, nullptr, 0, 1 };
   if (!*node) return false;

   byte bit = 0;
   if (!HUFFile::getbit(state.ifile, bit)) return false;
   state.size--;

   if (!bit) {

      if (!build_tree_leaf(state, &(*node)->child[0])) return false;
      if (!build_tree_leaf(state, &(*node)->child[1])) return false;

   } else {

      HUFFile::getbit(state.ifile, bit);
      state.size--;

      if (!bit) {
         if (!build_tree_leaf(state, &(*node)->child[0])) return false;
      } else {
         if (!build_tree_branch(state, &(*node)->child[0])) return false;
      }

      if (!build_tree_branch(state, &(*node)->child[1])) return false;
   }

   return true;
}

bool HuffmanCoding::decoding::build_tree_leaf(State& state, Node** node) {
   byte ch;
   if (!HUFFile::getbyte(state.ifile, ch)) return false;
   state.size -= 8;

   *node = new (std::nothrow) Node{ 0, nullptr, nullptr, nullptr, ch, 0 };
   return *node != nullptr;
}

void HuffmanCoding::decoding::destroy_tree(Node* root) {

   while (root) {

      if (root->child[0]) {
         root->next_node = root;
         root = root->child[0];
      }

      if (root->child[1]) {
         root->next_node = root;
         root = root->child[1];
      }

      Node* tmp = root->next_node;

      delete root;
      root = tmp;
   }
}