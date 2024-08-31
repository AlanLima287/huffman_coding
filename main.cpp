#include <iostream>

//#define DONT_PRINT_HUFFMAN_TREE
#include "huffman.h"

const char DEFAULT_DECODED_FILE_EXTENSIONS[4] = "txt";
const char ENCODED_FILE_EXTENSION[4] = "huf";

int main(int argc, char** argv) {

   if (argc < 2) {
      std::cerr << "No filepath has been given!";
      return 1; 
   }

   size_t filename_length;
   size_t extension_length = 0;
   char* cursor = argv[1];

   for (filename_length = 0; *cursor; filename_length++, cursor++) {
      if (*cursor == '.') extension_length = filename_length;
   }

   extension_length = filename_length - extension_length;
   filename_length -= extension_length - 1;

   if (!filename_length) {
      std::cerr << "Invalid filepath has been given!";
      return 1;
   }

   cursor = argv[1] + filename_length;
   bool is_encoded = false;

   if (sizeof(ENCODED_FILE_EXTENSION) == extension_length) {

      is_encoded = true;

      for (size_t i = 0; i < extension_length; i++) {
         if (cursor[i] != ENCODED_FILE_EXTENSION[i]) {
            is_encoded = false; break;
         }
      }
   }

   extension_length = sizeof(!is_encoded ? ENCODED_FILE_EXTENSION : DEFAULT_DECODED_FILE_EXTENSIONS);
   char* output_filename = new char[filename_length + extension_length + 1];

   cursor = (char*)(!is_encoded ? ENCODED_FILE_EXTENSION : DEFAULT_DECODED_FILE_EXTENSIONS);

   for (size_t i = 0; i < filename_length; i++)
      output_filename[i] = argv[1][i];

   for (size_t i = 0; i < extension_length; i++)
      output_filename[filename_length + i] = cursor[i];

   HuffmanCoding::encode(argv[1], output_filename);
   return 0;
}