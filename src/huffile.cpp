#include "huffile.h"

bool HUFFile::open(File& file, const char* filename, mode_t mode) {

   const char options[3][4] = { "wbx", "rb", "wb" };

   file.file = fopen(filename, options[mode >> 1]);
   if (!file.file) {
      print_file_opening_error(errno);
      return exit_t::opening_error;
   }

   if (setvbuf(file.file, nullptr, _IONBF, 0)) {
      fclose(file.file);
      return exit_t::buffering_error;
   }

   // If reading/writing bits, one more byte
   // if (mode & 0b10) file.buffer = new (std::nothrow) byte[PAGE_SIZE + 1];
   // else file.buffer = new (std::nothrow) byte[PAGE_SIZE];

   file.buffer = new (std::nothrow) byte[PAGE_SIZE];
   if (!file.buffer) {
      fclose(file.file);
      return exit_t::bad_allocation;
   }

   // If reading, load stuff
   if (~mode & 0b01) {
      file.size = fread(file.buffer, sizeof(byte), PAGE_SIZE, file.file);
      if (!file.size) return exit_t::empty_file;
   }

   file.cursor = 0;
   return exit_t::success;
}

void HUFFile::close(File& file) {
   delete[] file.buffer;
   fclose(file.file);
}

void HUFFile::rewind(File& file) {
   fseek(file.file, 0, SEEK_SET);
}

bool HUFFile::getchar(File& file, byte& ch) {
   if (file.cursor >= file.size) {

      if (feof(file.file)) return false;

      file.size = fread(file.buffer, sizeof(byte), PAGE_SIZE, file.file);
      if (!file.size) return false;

      file.cursor = 0;
   }

   ch = file.buffer[file.cursor++];
   return true;
}

void HUFFile::putbits(File& file, byte* base, uint64_t length) {
   const uint64_t page_size_bits = PAGE_SIZE * (sizeof(byte) << 3);
   
   while (true) {
      uint64_t limit = minimum(file.cursor + length, page_size_bits);

      for (uint64_t i = 0; file.cursor < limit; i++, length--) 
         BitTools::setbit(file.buffer, file.cursor++, BitTools::getbit(base, i));
      
      if (!length) return;

      fwrite(file.buffer, sizeof(byte), PAGE_SIZE, file.file);
      file.cursor = 0;
   }
}

bool HUFFile::getbits(File& file, byte* base, uint64_t length) {
   // const uint64_t page_size_bits = PAGE_SIZE * (sizeof(byte) << 3);
   // uint64_t i;

   // for (uint64_t i = 0; i < length; i++) {
   //    BitTools::setbit(base, i, BitTools::getbit(file.buffer, file.cursor++));
   // }
}

void HUFFile::putchar(File& file, byte ch) {
   if (file.cursor >= PAGE_SIZE) {
      fwrite(file.buffer, sizeof(byte), PAGE_SIZE, file.file);
      file.cursor = 0;
   }

   file.buffer[file.cursor++] = ch;
}

void HUFFile::flush(File& file) {
   fwrite(file.buffer, sizeof(byte), BitTools::size(file.cursor), file.file);
   file.cursor = 0;
}

/* Deprecated, for now */
/*
void HUFFile::putbits(File& file, byte* base, uint64_t length) {

   if (file.cursor + length >= PAGE_SIZE * (sizeof(byte) << 3)) {

      BitTools::putbitstr(file.buffer, file.cursor, base, PAGE_SIZE - file.cursor);

      base += PAGE_SIZE - file.cursor;
      length -= PAGE_SIZE - file.cursor;

      fwrite(file.buffer, sizeof(byte), PAGE_SIZE, file.file);

      BitTools::putbitstr(file.buffer, 0, base, length);
      file.cursor = length;

      return;
   }

   BitTools::putbitstr(file.buffer, file.cursor, base, length);
   file.cursor += length;
}

bool HUFFile::getbits(File& file, byte* base, uint64_t length) {}
*/