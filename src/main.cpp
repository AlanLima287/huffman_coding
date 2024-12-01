#include "base.h"
#include "huffman.h"
#include <iostream>

bool form_output_filename(const char*, char*&);
void cmd_help();

int main(int argc, char** argv) {

   system_specifics_setup();

   word width, height;
   get_terminal_dimensions(width, height);

   enum flag_t : byte {
      HELP      = 0b00001,
      ENCODE    = 0b00010,
      DECODE    = 0b00100,
      ALLOCATED = 0b01000,
      // SHOW_TREE = 0b10000,
   };

   byte flags = 0;
   const char* input_filename = nullptr;
   char* output_filename = nullptr;

   esc::color(esc::clr::YELLOW, esc::grnd::FOREGROUND);

   if (argc < 2) {
      print_in_column("Argumentos insuficientes!\n", 0, width);
      goto error;
   }

   // Process arguments
   for (int i = 1; i < argc; i++) {
      if (argv[i][0] == '-') {

         // Only one letter flags are going to be counted as flags
         if (!argv[i][1] || argv[i][2]) continue;

         switch (argv[i][1]) {
            case 'h': flags |= HELP; break;
            case 'e': flags |= ENCODE; break;
            case 'd': flags |= DECODE; break;
            // case 'i': flags |= INTERACTIVE; break;
         }

      } else {

         if (false);

         else if (!input_filename)
            input_filename = argv[i];
         else if (!output_filename)
            output_filename = argv[i];

      }
   }

   if (flags & HELP) {
      esc::reset();
      cmd_help();
      return 0;
   }

   // A input file must always be given
   if (!input_filename) {
      print_in_column("Um arquivo de saída precisa ser informado!\n", 0, width);
      goto error;
   }

   // !(~a & M) <=> a & M == M
   // Check whether both flags are active
   if (!(~flags & (ENCODE | DECODE))) {
      print_in_column("Parametros ambíguos!\n", 0, width);
      goto error;
   }

   // A output must be given when there are flags
   if (!output_filename && (flags & (ENCODE | DECODE))) {
      print_in_column("Um arquivo de saída precisa ser informado!\n", 0, width);
      goto error;
   }

   esc::reset();

   if (!flags) {
      output_filename = nullptr;
      bool is_encoded = form_output_filename(input_filename, output_filename);
         
      if (!output_filename) {
         std::cout.write(BAD_ALLOCATION, sizeof(BAD_ALLOCATION) - 1);
         return 1;
      }

      flags |= ALLOCATED | (is_encoded ? DECODE : ENCODE);
   }

   if (false);
   
   else if (flags & ENCODE) {
      if (!HuffmanCoding::encode(input_filename, output_filename)) {
         std::cout << "Algo deu errado ao codificar " << input_filename;
         goto error;
      }
   }
   
   else if (flags & DECODE) {
      if (!HuffmanCoding::decode(input_filename, output_filename)) {
         std::cout << "Algo deu errado ao decodificar " << input_filename;
         goto error;
      }
   }

   if (flags & ALLOCATED)
      free(output_filename);

   return 0;

error:
   esc::reset();
   return 1;
}

bool form_output_filename(const char* input_filename, char*& output_filename) {
   const char extension[] = ".huf";
   uint64_t length = sizeof(extension) - 1;

   uint64_t strlen;
   for (strlen = 0; input_filename[strlen]; strlen++);

   bool has_huf_extension = true;
   for (uint64_t i = 0; i < length; i++) {

      if (input_filename[strlen - length + i] != extension[i]) {
         has_huf_extension = false;
         break;
      }
   }

   if (length < strlen && has_huf_extension) {
      
      if (char* ptr = (char*)realloc(output_filename, (strlen - length) * sizeof(char))) 
         output_filename = ptr;
      else {
         if (output_filename)
            free(output_filename);
         output_filename = ptr;
         return true; // Bad Allocation
      }

      uint64_t i;
      for (i = 0; i < strlen - length; i++)
         output_filename[i] = input_filename[i];
      
      output_filename[strlen - length] = '\0';
      
      return true; // For decoding

   } else {

      if (char* ptr = (char*)realloc(output_filename, (strlen + length) * sizeof(char))) 
         output_filename = ptr;
      else {
         if (output_filename)
            free(output_filename);
         output_filename = ptr;
         return false; // Bad Allocation
      }
      
      for (uint64_t i = 0; i < strlen; i++)
         output_filename[i] = input_filename[i];

      for (uint64_t i = 0; i < length; i++)
         output_filename[strlen + i] = extension[i];

      output_filename[strlen + length] = '\0';

      return false; // For encoding
   }
}

void cmd_help() {
   word width, height;
   get_terminal_dimensions(width, height);

   print_in_column("clue -h\n", 0, width);
   print_in_column("Mostra essa tela\n", 3, width);

   print_in_column("clue <arquivo>\n", 0, width);
   print_in_column("Se <arquivo> = <nome>.huf, decodifica-se <arquivo> e põe os resultado em <nome>\n", 3, width);
   print_in_column("Caso contrário, codifica o arquivo e põe o resultado em <arquivo>.huf\n", 3, width);

   print_in_column("clue -d <arquivo de entrada> <arquivo de saída>\n", 0, width);
   print_in_column("Decodifica <arquivo de entrada> e põe o resultado em <arquivo de saída>\n", 3, width);

   print_in_column("clue -e <arquivo de entrada> <arquivo de saída>\n", 0, width);
   print_in_column("Codifica <arquivo de entrada> e põe o resultado em <arquivo de saída>\n", 3, width);
}