#pragma once

#include "base.h"

#include "huffman.h"
#include "../lib/bittools.h"

namespace HUFFile {

   const uint64_t PAGE_SIZE = 0x1000;

   enum exit_t : byte { success, file_already_exists, opening_error, buffering_error, bad_allocation };
   
   enum mode_t : byte {
      read      = 0b001,
      write     = 0b010,
      overwrite = 0b100,
   };

   typedef union {
      byte literal;
      struct {
         byte
            last_byte : 3, // The position of the last true bit in the last byte
            magic_hex : 4, // Not sure yet
            true_tree : 1; // The file contains 2 or more kinds of characters
      } header;
   } Flags;

   typedef struct {
      FILE* file; // File Pointer
      uint64_t size; // Amount of loaded byte
      uint64_t cursor; // A cursor inside the file buffer
      byte* buffer; // The file buffer
   } File;

   inline exit_t open(File&, const char* filename, mode_t mode);
   inline void close(File&);
   
   inline uint64_t file_size(File&);
   inline void rewind(File&);

   inline bool getchar(File&, byte&);
   inline void putchar(File&, byte);

   inline void putbits(File&, byte*, uint64_t);
   inline bool getbyte(File&, byte&);
   inline bool getbit(File&, byte&);
   
   inline void flushbits(File&);
   inline void flush(File&);
}