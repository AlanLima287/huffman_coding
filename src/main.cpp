#include "huffman.h"
#include <iostream>

int main(int argc, char** argv) {
   HuffmanCoding::encode("./rc/b.txt", "./rc/b.txt.huf");
   return 0;
}
