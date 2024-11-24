#include <iostream>
#include "huffman.h"

const char ENCODED_FILE_EXTENSION[4] = "huf";

int main(int argc, char** argv) {
   // int main() { int argc = 2; char argv[][10] = { "", "a.txt" }; // }

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
      std::cerr << "Invalid filename has been given!";
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

   if (is_encoded) {

      char* output_filename = new char[filename_length];
      std::cout << filename_length << ' ';

      for (size_t i = 0; i < filename_length - 1; i++)
         output_filename[i] = argv[1][i];
      
      output_filename[filename_length - 1] = '\0';

      std::cout << argv[1] << ' ' << output_filename;

      // HuffmanCoding::decode(argv[1], output_filename);
      delete[] output_filename;

   } else {

      filename_length += extension_length;
      char* output_filename = new char[filename_length + sizeof(ENCODED_FILE_EXTENSION)];
      std::cout << filename_length + sizeof(ENCODED_FILE_EXTENSION) << ' ';

      size_t i;
      for (i = 0; i < filename_length - 1; i++)
         output_filename[i] = argv[1][i];

      output_filename[filename_length - 1] = '.';

      cursor = output_filename + filename_length;
      for (i = 0; i < sizeof(ENCODED_FILE_EXTENSION); i++)
         *cursor++ = ENCODED_FILE_EXTENSION[i];

      std::cout.write(output_filename, filename_length + sizeof(ENCODED_FILE_EXTENSION));

      if (HuffmanCoding::encode(argv[1], output_filename))
         std::cout << "Everything might've gone fine!\n";
      else
         std::cerr << "Something went wrong!\n";

      delete[] output_filename;
   }

   return 0;
}