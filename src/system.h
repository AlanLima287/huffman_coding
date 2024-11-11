#pragma once

// Checks whether a some pointer is valid, if so, accesses some field in a 
// structure/object pointed by it, if not, then returns some default value

#ifndef __SYSTEM___SPECIFICS___
#define __SYSTEM___SPECIFICS___

// UTF-8, ASCII extended and pure ascii respectivily
// Assuming Linux terminals will support UTF-8

void get_terminal_dimensions(unsigned short&, unsigned short&);
inline bool system_specifics_setup();

#if defined(__linux__) || defined(__APPLE__)

#include <sys/ioctl.h>

void get_terminal_dimensions(unsigned short& width, unsigned short& height) {
   struct winsize win;
   ioctl(fileno(stdout), TIOCGWINSZ, &win);
   width = (unsigned short)(win.ws_col);
   height = (unsigned short)(win.ws_row);
}

inline bool system_specifics_setup() { return true; }

#elif defined(_WIN32) || defined(_WIN64)

#include <Windows.h>

void get_terminal_dimensions(unsigned short& width, unsigned short& height) {
   CONSOLE_SCREEN_BUFFER_INFO csbi;
   GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

   height = (unsigned short)(csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
   width = (unsigned short)(csbi.srWindow.Right - csbi.srWindow.Left + 1);
}

// The default code page, i.e, the encoding under which caracters are renderer,
// is not guarantied to be UTF-8, this fixes that. Also enables the processing of
// ANSI escape code sequences
inline bool system_specifics_setup() {
   HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
   if (handle == INVALID_HANDLE_VALUE) return false;

   if (!SetConsoleOutputCP(CP_UTF8)) return false;

   DWORD dwMode = 0;
   if (!GetConsoleMode(handle, &dwMode)) return false;

   dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
   if (!SetConsoleMode(handle, dwMode)) return false;

   return true;
}

#endif

#endif /* __SYSTEM___SPECIFICS___ */