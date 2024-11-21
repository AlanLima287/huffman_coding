#include "base.h"

#include "huffman.h"
#include "../lib/bittools.h"

namespace HUFFile {

   const uint64_t PAGE_SIZE = 0x1000;

   enum exit_t : byte { success, opening_error, buffering_error, file_already_exists, bad_allocation, empty_file };
   
   enum mode_t : byte {
      write          = 0b000,
      write_bits     = 0b001,
      read           = 0b010,
      read_bits      = 0b011,
      overwrite      = 0b100,
      overwrite_bits = 0b101,
   };

   typedef union {
      byte state;
      byte
         magic_hex : 4, // Not sure yet
         true_tree : 1, // The file contains 2 or more kinds of characters
         last_byte : 3; // The position of the last true bit in the last byte
   } Flags;

   typedef struct {
      FILE* file; // File Pointer
      uint64_t size; // Amount of loaded byte
      uint64_t cursor; // A cursor inside the file buffer
      byte* buffer; // The file buffer
   } File;

   inline bool open(File&, const char* filename, mode_t mode);
   inline void close(File&);
   
   inline void rewind(File&);

   inline bool getchar(File&, byte&);
   inline void putchar(File&, byte);

   inline void putbits(File&, byte*, uint64_t);
   inline bool getbits(File&, byte*, uint64_t);
   
   inline void flush(File&);
}