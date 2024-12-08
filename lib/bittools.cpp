#include "bittools.h"

BitTools::byte* BitTools::construct(uint64_t length, byte set) {
   length = BitTools::size(length);

   byte* base = new (std::nothrow) byte[length];
   if (!base) return nullptr;

   for (uint64_t i = 0; i < length; i++)
      base[i] = set;

   return base;
}

BitTools::byte* BitTools::initialize(byte* base, uint64_t length, byte set) {
   length = BitTools::size(length);

   for (uint64_t i = 0; i < length; i++)
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

void BitTools::printin(byte* base, uint64_t size) {
   for (uint64_t i = 0; i < size; i++)
      std::cout.put(getbit(base, i) ? '1' : '0');
}

void BitTools::print(byte* base, uint64_t size, uint64_t pad) {
   if (!size) return;
   size--;

   for (uint64_t i = 0, j = 0; i <= size; i++) {
      std::cout.put(getbit(base, size - i) ? '1' : '0');
      if (++j >= pad) { std::cout.put(' '); j = 0; }
   }
}

void BitTools::printin(byte* base, uint64_t size, uint64_t pad) {
   for (uint64_t i = 0, j = 0; i < size; i++) {
      std::cout.put(getbit(base, i) ? '1' : '0');
      if (++j >= pad) { std::cout.put(' '); j = 0; }
   }
}

#if 0 && defined(__INTRIN_H_) && __HAS__INTRINSICS____

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

inline bool BitTools::setbit(byte* base, uint64_t pos, bool bit_value) {
   byte* word = base + (pos >> SHIFT);
   int64_t offset = 1 << (pos & MASK);

   bool bit = *word & offset;
   if (bit_value) *word |= offset;
   else *word &= ~offset;

   return bit;
}

inline void BitTools::putword(byte* base, uint64_t pos, byte iword) {
   byte* word = base + (pos >> SHIFT);
   pos &= MASK;

   *word &= NATOM << (POWER - pos);
   *word |= iword << pos;

   word++;

   *word &= NATOM >> pos;
   *word |= iword >> (POWER - pos);
}

inline BitTools::byte BitTools::getword(byte* base, uint64_t pos) {
   byte* word = base + (pos >> SHIFT);
   byte iword = 0;
   
   pos &= MASK;

   iword |= *word++ >> pos;
   iword |= *word << (POWER - pos);

   return iword;
}

inline void BitTools::putbitstr(byte* base, uint64_t pos, byte* bitstr, uint64_t length) {
   uint64_t count = length >> SHIFT;

   for (uint64_t i = 0; i < count; i++, pos += POWER) {
      putword(base, pos, *bitstr++);
   }

   length = (length & MASK) + pos;
   
   for (uint64_t i = 1ull; pos < length; i <<= 1, pos++) {
      if (*bitstr & i) setbit_1(base, pos);
      else setbit_0(base, pos);
   }
}

inline void BitTools::getbitstr(byte* base, uint64_t pos, byte* bitstr, uint64_t length) {
   uint64_t count = length >> SHIFT;

   for (uint64_t i = 0; i < count; i++, pos += POWER) {
      *bitstr++ = getword(base, pos);
   }

   *bitstr = getword(base, pos);
   *bitstr &= NATOM >> (POWER - (length & MASK));
}