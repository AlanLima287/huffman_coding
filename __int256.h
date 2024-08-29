#undef UINT64_MAX

class __int256 {
   using uint8 = unsigned char;
   using uint64 = unsigned long long;

   static const uint64 UINT64_SIGN = 0x8000000000000000;
   static const uint64 UINT64_MAX = ~0;

private:
   uint64 base[4];

public:
   __int256() { base[0] = 0; base[1] = 0; base[2] = 0; base[3] = 0; }
   
   __int256(uint64 num) {
      base[0] = num;
      if (num & UINT64_SIGN) {
         base[1] = UINT64_MAX;
         base[2] = UINT64_MAX;
         base[3] = UINT64_MAX;
      }
   }

   __int256(const __int256& num) {
      base[0] = num.base[0];
      base[1] = num.base[1];
      base[2] = num.base[2];
      base[3] = num.base[3];
   }

   __int256 operator=(const __int256& num) {
      base[0] = num.base[0];
      base[1] = num.base[1];
      base[2] = num.base[2];
      base[3] = num.base[3];
      return num;
   }

   bool __getbit(uint8 pos) {
      return base[(pos & 0xC0) >> 6] & (1ull << (pos & 0x3F));
   }

   void __setbit_0(uint8 pos) {
      base[(pos & 0xC0) >> 6] &= ~(1ull << (pos & 0x3F));
   }

   void __setbit_1(uint8 pos) {
      base[(pos & 0xC0) >> 6] |= 1ull << (pos & 0x3F);
   }
};
