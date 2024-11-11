#pragma once

/*
* Author
*    Alan Lima (https://github.com/AlanLima287/)
*
* BitTools
*    It's a not precompiled C++ library with wrapper function
*    for especific assembly instructions (BT, BTC, BTR, BTS)
*    or its behavior
*/

#ifndef __BIT_TOOLS_
#define __BIT_TOOLS_

#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <new>

#if __has_include("intrin.h") && (defined(_WIN32) || defined(_WIN64))
   #define __HAS__INTRINSICS____ 1
   #include <intrin.h>
#endif

namespace BitTools {

   typedef unsigned char byte;

   const byte SHIFT = sizeof(byte) >= 4 ? ((sizeof(byte) >> 2) + 4) : (sizeof(byte) + 2);
   const uint64_t POWER = 1ull << SHIFT;
   const uint64_t MASK = POWER - 1;

   byte* construct(uint64_t, byte = 0);
   inline void destruct(byte*);

   // BT (Bit Test)
   inline bool getbit(byte*, uint64_t);

   // BTC (Bit Test and Complement)
   inline bool flipbit(byte*, uint64_t);

   // BTR (Bit Test and Reset)
   inline bool setbit_0(byte*, uint64_t);

   // BTS (Bit Test and Set)
   inline bool setbit_1(byte*, uint64_t);

   // Fills the boolean array with a byte-wise pattern
   byte* initialize(byte*, uint64_t, byte = 0);

   inline uint64_t size(uint64_t size) { return (size + MASK) >> SHIFT; }

   inline void putword(byte*, uint64_t, byte);
   inline byte getword(byte*, uint64_t);

   // Print the bits
   template <typename type_t>
   void print(type_t, uint64_t);
   
   void print(byte*, uint64_t);
   void print(byte*, uint64_t, uint64_t);
};

#include "bittools.cpp"

#ifdef __HAS__INTRINSICS____
   #undef __HAS__INTRINSICS____
#endif

#endif /* __BIT_TOOLS_ */