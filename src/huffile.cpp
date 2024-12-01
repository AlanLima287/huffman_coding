#include "huffile.h"

HUFFile::exit_t HUFFile::open(File& file, const char* filename, mode_t mode) {

   switch (mode) {
      case mode_t::read:
         file.file = fopen(filename, "rb");
         break;

      case mode_t::write:
         if (file_exists(filename))
            return exit_t::file_already_exists;

      case mode_t::overwrite:
         file.file = fopen(filename, "wb");
         break;
   }

   if (!file.file) {
      print_file_opening_error(errno);
      return exit_t::opening_error;
   }

   if (setvbuf(file.file, nullptr, _IONBF, 0)) {
      fclose(file.file);
      return exit_t::buffering_error;
   }

   file.buffer = new (std::nothrow) byte[PAGE_SIZE];
   if (!file.buffer) {
      fclose(file.file);
      return exit_t::bad_allocation;
   }

   file.cursor = 0;
   file.size = 0;

   return exit_t::success;
}

void HUFFile::close(File& file) {
   delete[] file.buffer;
   fclose(file.file);
}

uint64_t HUFFile::file_size(File& file) {
   uint64_t cursor = ftell(file.file);
   fseek(file.file, 0, SEEK_END);

   uint64_t size = ftell(file.file);
   fseek(file.file, cursor, SEEK_SET);

   return size;
}


void HUFFile::rewind(File& file) {
   fseek(file.file, 0, SEEK_SET);

   file.cursor = 0;
   file.size = 0;
}

bool HUFFile::getchar(File& file, byte& ch) {
   if (file.cursor >= file.size) {

      file.size = fread(file.buffer, sizeof(byte), PAGE_SIZE, file.file);
      if (!file.size) return false;

      file.cursor = 0;
   }

   ch = file.buffer[file.cursor++];
   return true;
}

void HUFFile::putbits(File& file, byte* base, uint64_t length) {
   uint64_t page_size_bits = PAGE_SIZE * (sizeof(byte) << 3);
   uint64_t i = 0;

   while (true) {
      uint64_t limit = minimum(file.cursor + length, page_size_bits);

      for (; file.cursor < limit; i++, length--) 
         BitTools::setbit(file.buffer, file.cursor++, BitTools::getbit(base, i));
      
      if (!length) return;

      fwrite(file.buffer, sizeof(byte), PAGE_SIZE, file.file);
      file.cursor = 0;
   }
}

bool HUFFile::getbyte(File& file, byte& base) {
   
   base = 0;
   byte ch;

   for (uint8_t i = 0; i < 8; i++) {
      if (!getbit(file, ch)) return false;
      base |= ch << i;
   }

   return true;
}

bool HUFFile::getbit(File& file, byte& bit) {
   if (file.cursor >= file.size) {

      file.size = fread(file.buffer, sizeof(byte), PAGE_SIZE, file.file);
      if (!file.size) return false;

      file.size <<= 3;
      file.cursor = 0;
   }

   bit = BitTools::getbit(file.buffer, file.cursor++) ? 1 : 0;
   return true;
}

void HUFFile::putchar(File& file, byte ch) {
   if (file.cursor >= PAGE_SIZE) {
      fwrite(file.buffer, sizeof(byte), PAGE_SIZE, file.file);
      file.cursor = 0;
   }

   file.buffer[file.cursor++] = ch;
}

void HUFFile::flushbits(File& file) {
   if (file.cursor) {
      if (file.cursor & 0b111) {
         file.buffer[file.cursor >> 3] &= byte(0xFF >> (8 - (file.cursor & 0b111)));
      }

      fwrite(file.buffer, sizeof(byte), BitTools::size(file.cursor), file.file);
      file.cursor = 0;
   }
}

void HUFFile::flush(File& file) {
   if (file.cursor) {
      fwrite(file.buffer, sizeof(byte), file.cursor, file.file);
      file.cursor = 0;
   }
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