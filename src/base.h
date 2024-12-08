#pragma once

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#include <iostream>

#include "../lib/terminal.h"
#include "system.h"

typedef unsigned char      byte;
typedef unsigned short     word;
typedef unsigned long      dword;
typedef unsigned long long qword;

#define minimum(a, b) (((a) < (b)) ? (a) : (b))
#define maximum(a, b) (((a) > (b)) ? (a) : (b))

const char BAD_ALLOCATION[] = "\e[37;41;1mErro Fatal:\e[0;31m Má Alocação\e[m";

void print_character(byte ch) {

   /*
   * Character printing convention:
   *
   *    num ∊ [0, 6]: "\{num}"
   *    num ∊ [7, D]:
   *       num = 7: "\a",
   *       num = 8: "\b",
   *       num = 9: "\t",
   *       num = A: "\n",
   *       num = B: "\v",
   *       num = C: "\f",
   *       num = D: "\r";
   *    num ∊ [E, 1B) ∪ (1B, 1F] ∪ [7F, FF]: "\x{num}"
   *    num ∊ (20, 7E]: (char)num
   *    num = 1B: "\e"
   *    num = 20: ' '
   */

   // The following notes show the recorded branching behavior
   if (0x20 > ch || ch >= 0x7f) { // [0, 20) ∪ [7F, FF]

      putchar('\\');

      if (ch < 0x07) putchar('0' | ch);                   // [0, 7)
      else if (ch <= 0x0D) putchar("abtnvfr"[ch - 0x07]); // [7, D]
      else if (ch == 0x1B) putchar('e');                  // {1B}
      else printf("x%02hhx", ch);                         // [E, 1B) ∪ (1B, 1F] ∪ [7F, FF]
   }

   else if (ch == 0x20) printf("' '");                    // {20}
   else putchar(ch);                                      // (20, 7F)
}

void print_file_opening_error(int err) {
   switch (err) {
      case EACCES:       printf("Permissão negada. Você não tem permissão para acessar este arquivo.\n"); break;
      case EEXIST:       printf("Arquivo já existe. Não é possível criar um novo arquivo com o mesmo nome.\n"); break;
      case EFAULT:       printf("Endereço inválido. O caminho do arquivo aponta para fora do seu espaço de endereço acessível.\n"); break;
      case EFBIG:        printf("Arquivo muito grande. O arquivo excede o tamanho máximo permitido neste sistema de arquivos.\n"); break;
      case EINTR:        printf("Operação interrompida por um sinal.\n"); break;
      case EINVAL:       printf("Argumento inválido. Verifique o modo de abertura ou as flags usadas.\n"); break;
      case EIO:          printf("Erro de E/S. Houve um problema com o hardware ou dispositivo de armazenamento.\n"); break;
      case EISDIR:       printf("É um diretório. Você tentou abrir um diretório como se fosse um arquivo.\n"); break;
      case EMFILE:       printf("Muitos arquivos abertos. O processo atingiu seu limite de descritores de arquivo.\n"); break;
      case ENAMETOOLONG: printf("Nome de arquivo muito longo. O nome do arquivo excede o limite de comprimento definido pelo sistema.\n"); break;
      case ENFILE:       printf("Muitos arquivos abertos no sistema. O limite do sistema foi atingido.\n"); break;
      case ENOENT:       printf("Arquivo ou diretório não encontrado. O arquivo especificado não existe.\n"); break;
      case ENOMEM:       printf("Memória insuficiente. Memória insuficiente para alocar um novo descritor de arquivo.\n"); break;
      case ENOSPC:       printf("Sem espaço em disco. O dispositivo de armazenamento está cheio.\n"); break;
      case ENOTDIR:      printf("Não é um diretório. Um componente do caminho não é um diretório.\n"); break;
      case EROFS:        printf("Sistema de arquivos somente leitura. Não é possível escrever em um arquivo em um sistema de arquivos somente leitura.\n"); break;
      case ETXTBSY:      printf("Arquivo de texto ocupado. O arquivo está aberto para escrita por outro processo.\n"); break;
      default:           printf("Ocorreu um erro desconhecido. Código de erro: %d\n", err); break;
   }
}

uint64_t print_in_column(const char* text, word start, word end) {
   esc::move_to(start);

   uint64_t length = 0;
   uint64_t index = 0;
   uint64_t lines = 0;

   word width = end - start;

   while (true) {

      switch (text[index]) {
         case '\0':
            if (index > 0) {
               std::cout.write(text, index);
               lines++;
            }

            return lines;

         case '\n':
            if (index > 0) {
               std::cout.write(text, ++index);
               text += index;

               length = 0;
               index = 0;
            } else {
               std::cout.put('\n');
               text++;
            }

            esc::move_to(start);
            lines++;

            continue;

         case ' ':
            length = index;
            break;
      }

      if (index >= width) {

         uint8_t inc = 0;

         if (!length) { length = width; index = 0; } else { index -= length; inc = 1; }

         std::cout.write(text, length);
         text += length + inc;
         length = 0;

         std::cout.put('\n');
         esc::move_to(start);
         lines++;

         continue;
      }

      index++;
   }
}