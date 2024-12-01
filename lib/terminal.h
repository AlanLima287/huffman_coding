#pragma once

/*
* Author
*    Alan Lima (https://github.com/AlanLima287/)
*
* Terminal
*    It's a not precompiled C++ library that intermediates interaction
*    with a terminal that supports ANSI escape codes.
*
* Further reading and References
*    https://en.wikipedia.org/wiki/ANSI_escape_code
*    https://vt100.net/docs/vt510-rm/chapter4.html
*/

#include <stdio.h>

namespace esc {

   typedef unsigned char  byte;
   typedef unsigned short word;
   typedef unsigned long  rgb;

   enum csi : byte {
      PRE_CURSOR = 0,
      POS_CURSOR = 1,
      ENTIRE     = 2,
      CLEANSE    = 3,

      UP      = 0,
      DOWN    = 1,
      FORWARD = 2,
      BACK    = 3,

      NEXT     = 1,
      PREVIOUS = 0,
   };

   enum clr : byte {
      BLACK   = 30,
      RED     = 31,
      GREEN   = 32,
      YELLOW  = 33,
      BLUE    = 34,
      MAGENTA = 35,
      CYAN    = 36,
      WHITE   = 37,

      RESET = 39,

      GRAY           = 90,
      BRIGHT_BLACK   = 90,
      BRIGHT_RED     = 91,
      BRIGHT_GREEN   = 92,
      BRIGHT_YELLOW  = 93,
      BRIGHT_BLUE    = 94,
      BRIGHT_MAGENTA = 95,
      BRIGHT_CYAN    = 96,
      BRIGHT_WHITE   = 97,
   };

   enum grnd : byte {
      FOREGROUND = 38,
      BACKGROUND = 48,
   };

   enum smk : byte {
      NONE      = 0x00,
      BOLD      = 0x01,
      LIGHT     = 0x02,
      ITALIC    = 0x04,
      UNDERLINE = 0x08,
      STRIKE    = 0x10,
      INVERT    = 0x20,
   };

   const rgb DEFAULT = 0xFF000000;

   /* CSI (Control Sequence Introducer) */

   void clear(csi = ENTIRE);
   void clear_line(csi = ENTIRE);

   void move(csi, word = 1);
   void move_line(csi, word = 1);

   void scroll(csi, word = 1);

   void move_to(word, word);
   void move_to(word);

   /* SGR (Select Graphic Rendition) */

   void reset();

   void color(clr, grnd);
   void color(grnd, byte, byte, byte);
   void color(grnd, rgb);
   void color();

   void bold(bool = true);
   void light(bool = true);
   void italic(bool = true);
   void underline(bool = true);
   void invert(bool = true);
   void strike(bool = true);

   void style(byte, clr, clr);
   void style(byte, rgb, rgb = DEFAULT);
};

#include "terminal.cpp"