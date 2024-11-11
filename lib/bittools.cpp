#include "bittools.h"

BitTools::byte* BitTools::construct(uint64_t size, byte set) {
   size = BitTools::size(size);

   byte* base = new (std::nothrow) byte[size];
   if (!base) return nullptr;

   for (uint64_t i = 0; i < size; i++)
      base[i] = set;

   return base;
}

BitTools::byte* BitTools::initialize(byte* base, uint64_t size, byte set) {
   size = BitTools::size(size);

   for (uint64_t i = 0; i < size; i++)
      base[i] = set;

   return base;
}

inline void BitTools::destruct(byte* base) {
   delete[] base;
}

template <typename type_t>
void print(type_t number, uint64_t size) {
   for (size = 1ull << size - 1; size; size >>= 1)
      std::cout.put((number & size) ? '1' : '0');
}

void BitTools::print(byte* base, uint64_t size) {
   if (size--) {
      do std::cout.put(getbit(base, size) ? '1' : '0');
      while (size--);
   }
}

void BitTools::print(byte* base, uint64_t size, uint64_t pad) {
   if (!size) return;

   size--;
   pad = (1ull << pad) - 1;

   for (uint64_t i = 0; i <= size; i++) {
      std::cout.put(getbit(base, size - i) ? '1' : '0');
      if (!(~i & pad)) std::cout.put(' ');
   }
}

#if defined(__INTRIN_H_) && __HAS__INTRINSICS____

inline bool BitTools::getbit(byte* base, uint64_t pos) { return _bittest64((const long long*)(base + (pos >> SHIFT)), pos & MASK); }
inline bool BitTools::flipbit(byte* base, uint64_t pos) { return _bittestandcomplement64((long long*)(base + (pos >> SHIFT)), pos & MASK); }
inline bool BitTools::setbit_0(byte* base, uint64_t pos) { return _bittestandreset64((long long*)(base + (pos >> SHIFT)), pos & MASK); }
inline bool BitTools::setbit_1(byte* base, uint64_t pos) { return _bittestandset64((long long*)(base + (pos >> SHIFT)), pos & MASK); }

#else

inline bool BitTools::getbit(byte* base, uint64_t pos) {
   return base[pos >> SHIFT] & (1 << (pos & MASK));
}

inline bool BitTools::flipbit(byte* base, uint64_t pos) {
   byte* word = base + (pos >> SHIFT);
   int64_t offset = 1 << (pos & MASK);

   bool bit = *word & offset;
   *word ^= offset;

   return bit;
}

inline bool BitTools::setbit_0(byte* base, uint64_t pos) {
   byte* word = base + (pos >> SHIFT);
   int64_t offset = 1 << (pos & MASK);

   bool bit = *word & offset;
   *word &= ~offset;

   return bit;
}

inline bool BitTools::setbit_1(byte* base, uint64_t pos) {
   byte* word = base + (pos >> SHIFT);
   int64_t offset = 1 << (pos & MASK);

   bool bit = *word & offset;
   *word |= offset;

   return bit;
}

#endif /* defined(__INTRIN_H_) && __HAS__INTRINSICS____ */

inline void BitTools::putword(byte* base, uint64_t pos, byte iword) {
   byte* word = base + (pos >> SHIFT);

   *word++ |= iword << (pos & MASK);
   *word |= iword >> ((POWER - pos) & MASK);
}

inline BitTools::byte BitTools::getword(byte* base, uint64_t pos) {
   byte* word = base + (pos >> SHIFT);
   byte iword = 0;

   iword |= *word++ >> (pos & MASK);
   iword |= *word << ((POWER - pos) & MASK);

   return iword;
}