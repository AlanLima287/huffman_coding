#include "huffman.h"
#include <iostream>

int main(int argc, char** argv) {
   
   system_specifics_setup();

   if (argc <= 1) {
      HuffmanCoding::decode("./out.huf", "./in.txt");
   } else {
      HuffmanCoding::encode(argv[1], "./out.huf");
   }

   return 0;
}
