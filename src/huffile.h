#include <stdio.h>
#include "huffman.h"

namespace HUFFile {
   typedef union {
      byte state;
      byte
         true_tree : 1, // The file contains 2 or more kinds of characters
         truly_encoded : 1, // The file is encoded under the default encoding
         b5 : 1,
         b4 : 1,
         b3 : 1,
         size_order : 3; // The amount of bytes (-1) used to store the number of character in the original file
   } HUFFileFlags;

   typedef struct {
      FILE* file; // File Pointer
      uint64_t size; // Amount of loaded byte
      uint64_t length; // Length of the whole file
      uint64_t cursor; // A cursor inside the file buffer
      byte* buffer; // The file buffer
   } HUFIFile;

   bool getchar(HUFIFile&, byte&); // for encoding
   bool getbits(HUFIFile&, byte&); // for decoding

   typedef struct {
      FILE* file; // File Pointer
      uint64_t cursor; // A cursor inside the file buffer
      byte* buffer; // The file buffer
   } HUFOFile;

   void putchar(HUFOFile&, byte); // for decoding
   void putbits(HUFOFile&, byte); // for encoding
}

