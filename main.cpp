#include <iostream>
#include "huffman.h"

const char DEFAULT_DECODED_FILE_EXTENSIONS[4] = "txt";
const char ENCODED_FILE_EXTENSION[4] = "huf";

int main(int argc, const char** argv) {
   // int main() { int argc = 2; char argv[][10] = { "", "a.txt" };

   if (argc < 2) {
      std::cerr << "No filepath has been given!";
      return 1;
   }

   size_t filename_length;
   size_t extension_length = 0;
   const char* cursor = argv[1];

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
      extension_length = sizeof(DEFAULT_DECODED_FILE_EXTENSIONS);
      cursor = DEFAULT_DECODED_FILE_EXTENSIONS;
   }
   else {
      extension_length = sizeof(ENCODED_FILE_EXTENSION);
      cursor = ENCODED_FILE_EXTENSION;
   }

   char* output_filename = new char[filename_length + extension_length];

   size_t i = 0;
   for (; i < filename_length; i++)
      output_filename[i] = argv[1][i];

   do {
      output_filename[i++] = *cursor;
   } while (*cursor++);

   if (HuffmanCoding::encode(argv[1], output_filename))
      std::cout << "Everything might've gone fine!\n";
   else
      std::cerr << "Something went wrong!\n";

   delete[] output_filename;
   return 0;
}