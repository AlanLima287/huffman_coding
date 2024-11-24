#pragma once

#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#include <iostream>

#include "system.h"

typedef unsigned char      byte;
typedef unsigned short     word;
typedef unsigned long      dword;
typedef unsigned long long qword;

#define minimum(a, b) (((a) < (b)) ? (a) : (b))
#define maximum(a, b) (((a) > (b)) ? (a) : (b))

void print_character(char ch) {

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

      if (ch < 0x07) putchar('0' + ch);                  // [0, 7)
      else if (ch < 0x0E) putchar("abtnvfr"[ch - 0x07]); // [7, E)
      else if (ch < 0x1B) putchar('e');                  // {1B}
      else printf("x%02hhx", ch);                        // [E, 1B) ∪ (1B, 1F] ∪ [7F, FF]
   }

   else if (ch == 0x20) printf("' '", ch); // {20}
   else putchar(ch);                       // (20, 7F)
}

void print_file_opening_error(errno_t err) {
   switch (err) {
      case EACCES:       printf("Permission denied. You don't have the right permissions to access this file.\n"); break;
      case EEXIST:       printf("File already exists. Cannot create a new file with the same name.\n"); break;
      case EFAULT:       printf("Bad address. The file path points outside your accessible address space.\n"); break;
      case EFBIG:        printf("File too large. The file exceeds the maximum allowed size on this file system.\n"); break;
      case EINTR:        printf("Operation interrupted by a signal.\n"); break;
      case EINVAL:       printf("Invalid argument. Check the open mode or flags used.\n"); break;
      case EIO:          printf("I/O error. There was an issue with the hardware or storage device.\n"); break;
      case EISDIR:       printf("Is a directory. You tried to open a directory as if it were a file.\n"); break;
      case EMFILE:       printf("Too many open files. The process has reached its file descriptor limit.\n"); break;
      case ENAMETOOLONG: printf("File name too long. The file name exceeds the system-defined length limit.\n"); break;
      case ENFILE:       printf("Too many open files in the system. The system-wide limit has been reached.\n"); break;
      case ENOENT:       printf("No such file or directory. The specified file does not exist.\n"); break;
      case ENOMEM:       printf("Out of memory. Insufficient memory to allocate a new file descriptor.\n"); break;
      case ENOSPC:       printf("No space left on device. The storage device is full.\n"); break;
      case ENOTDIR:      printf("Not a directory. A component of the path is not a directory.\n"); break;
      case EROFS:        printf("Read-only file system. Cannot write to a file on a read-only file system.\n"); break;
      case ETXTBSY:      printf("Text file busy. The file is open for writing by another process.\n"); break;
      default:           printf("An unknown error occurred. Error code: %d\n", err); break;
   }
}