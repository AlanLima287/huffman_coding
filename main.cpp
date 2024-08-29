#include <iostream>
#include "huffman.h"

const char DEFAULT_DECODED_FILE_EXTENSIONS[4] = "txt";
const char ENCODED_FILE_EXTENSION[4] = "huf";

int main(int argc, char** argv) {

   if (argc < 2) {
      std::cerr << "No filepath has been given!";
      return 1;
   }

   size_t filepath_length;
   size_t extension_length = 0;
   char* cursor = argv[1];

   for (filepath_length = 0; *cursor; filepath_length++, cursor++) {
      if (*cursor == '.') extension_length = filepath_length;
   }

   extension_length = filepath_length - extension_length;
   filepath_length -= extension_length - 1;

   if (!filepath_length) {
      std::cerr << "Invalid filepath has been given!";
      return 1;
   }

   cursor = argv[1] + filepath_length;
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
   char* output_filepath = new char[filepath_length + extension_length + 1];

   cursor = (char*)(!is_encoded ? ENCODED_FILE_EXTENSION : DEFAULT_DECODED_FILE_EXTENSIONS);

   for (size_t i = 0; i < filepath_length; i++)
      output_filepath[i] = argv[1][i];

   for (size_t i = 0; i <= extension_length; i++)
      output_filepath[filepath_length + i] = *cursor++;

   HuffmanCoding::encode(argv[1], output_filepath);
   return 0;
}