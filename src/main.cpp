#include "base.h"
#include "huffman.h"
#include <iostream>

int main(int argc, char** argv) {

   system_specifics_setup();

   word width, height;
   get_terminal_dimensions(width, height);

   enum flag_t : byte {
      HELP = 0b0001,
      ENCODE = 0b0010,
      DECODE = 0b0100,
      SHOW_TREE = 0b1000,
   };

   byte flags = 0;
   bool allocated = false;

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
            case 's': flags |= SHOW_TREE; break;
         }

      } else {

         if (false);

         else if (!input_filename)
            input_filename = argv[i];
         else if (!output_filename)
            output_filename = argv[i];

         else {
            print_in_column("Arquivos demais foram fornecidos!\n", 0, width);
            goto error;
         }
      }
   }

   if (flags & HELP) {
      esc::reset();

      word width, height;
      get_terminal_dimensions(width, height);

      print_in_column("clue -h\n", 0, width);
      print_in_column("Mostra essa tela\n", 3, width);

      print_in_column("clue <arquivo>\n", 0, width);
      print_in_column("Se <arquivo> = <nome>.huf, decodifica-se <arquivo> e põe os resultado em <nome>,\n", 3, width);
      print_in_column("caso contrário, codifica-se <arquivo> e põe o resultado em <arquivo>.huf.\n", 3, width);

      print_in_column("clue -d <arquivo de entrada> <arquivo de saída>\n", 0, width);
      print_in_column("Decodifica <arquivo de entrada> e põe o resultado em <arquivo de saída>.\n", 3, width);

      print_in_column("clue -e <arquivo de entrada> <arquivo de saída>\n", 0, width);
      print_in_column("Codifica <arquivo de entrada> e põe o resultado em <arquivo de saída>.\n", 3, width);

      print_in_column("clue -s ...\n", 0, width);
      print_in_column("Adicinando -s a uma chamada mostrará a árvore de Huffman gerada.\n", 3, width);
      return 0;
   }

   /* Improper input handling */ {

      // A input file must always be given
      if (!input_filename) {
         print_in_column("Um arquivo de entrada precisa ser informado!\n", 0, width);
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
   }

   // Either has not DECODE nor ENCODE flag or has ENCODE flag
   if (!(flags & DECODE) || (flags & ENCODE)) {

      const char extension[] = ".huf";
      uint64_t ext_length = sizeof(extension) - 1;

      if (flags & ENCODE) { // make sure the output has a .huf extension

         uint64_t length = strlen(output_filename);
         bool has_huf_extension = !strcmp(extension, output_filename + length - ext_length);

         if (!has_huf_extension) {

            char* new_output_filename = new (std::nothrow) char[length + ext_length + 1];
            if (!new_output_filename) {
               std::cout.write(BAD_ALLOCATION, sizeof(BAD_ALLOCATION) - 1);
               return 1;
            }

            strncpy(new_output_filename, output_filename, length);
            strncpy(new_output_filename + length, extension, ext_length);
            new_output_filename[length + ext_length] = '\0';

            output_filename = new_output_filename;
            allocated = true;
         }

      } else { // it has to form a name for the output file

         uint64_t length = strlen(input_filename);
         bool has_huf_extension = !strcmp(extension, input_filename + length - ext_length);

         if (ext_length < length && has_huf_extension) {

            output_filename = new (std::nothrow) char[length - ext_length + 1];
            if (!output_filename) {
               std::cout.write(BAD_ALLOCATION, sizeof(BAD_ALLOCATION) - 1);
               return 1;
            }

            strncpy(output_filename, input_filename, length - ext_length);
            output_filename[length - ext_length] = '\0';

            flags |= DECODE;

         } else {

            output_filename = new (std::nothrow) char[length + ext_length + 1];
            if (!output_filename) {
               std::cout.write(BAD_ALLOCATION, sizeof(BAD_ALLOCATION) - 1);
               return 1;
            }

            strncpy(output_filename, input_filename, length);
            strncpy(output_filename + length, extension, ext_length);
            output_filename[length + ext_length] = '\0';

            flags |= ENCODE;
         }

         allocated = true;
      }
   }

   if (flags & ENCODE) {

      if (!HuffmanCoding::encode(input_filename, output_filename, flags & SHOW_TREE)) {
         std::cout << "Algo deu errado ao codificar " << input_filename;
         std::cout.put('\n');
         goto error;
      }

   } else if (flags & DECODE) {

      if (!HuffmanCoding::decode(input_filename, output_filename, flags & SHOW_TREE)) {
         std::cout << "Algo deu errado ao decodificar " << input_filename;
         std::cout.put('\n');
         goto error;
      }

   }

   if (allocated)
      delete[] output_filename;

   return 0;

error:
   esc::reset();
   return 1;
}