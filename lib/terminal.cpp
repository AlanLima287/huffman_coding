#include "terminal.h"

/* @param mode
*    PRE_CURSOR: Clear from the cursor to the end of the screen;
*    POS_CURSOR: Clear from the cursor to the beginning of the screen;
*    ENTIRE: Clear the entire screen.
*    CLEANSE: Clear the entire screen and delete all lines saved in the scrollback.
*/
void esc::clear(csi mode) {
   printf("\e[%hhuJ", mode);
}

/* @param mode
 *    PRE_CURSOR: Clear from the cursor to the end of the line;
 *    POS_CURSOR: Clear from the cursor to the beginning of the line;
 *    ENTIRE: Clear the entire line.
*/
void esc::clear_line(csi mode) {
   printf("\e[%hhuK", mode);
}

/* @param direction
 *    UP: Move the cursor `amount` of cells upwards;
 *    DOWN: Move the cursor `amount` of cells downwards;
 *    FORWARD: Move the cursor `amount` of cells forward;
 *    BACK: Move the cursor `amount` of cells backwards.
 *
 * @param amount
 *    defines the amount (default 1) of cells the cursor will move in the given
 *    direction.
*/
void esc::move(csi direction, word amount) {
   printf("\e[%hu%c", amount, direction + 'A');
}

/* @param direction
 *    NEXT: Move the cursor to the beginning of the `amount`-th next line;
 *    PREVIOUS: Move the cursor to the beginning of the `amount`-th previous line;
 *
 * @param amount
 *    defines the amount (default 1) of lines the cursor will move in the given
 *    direction.
*/
void esc::move_line(csi direction, word amount) {
   printf("\e[%hu%c", amount, direction + 'E');
}

/* @param direction
 *    UP: Scroll page up by `amount` lines;
 *    DOWN: Scroll page down by `amount` lines;
 *
 * @param amount
 *    defines the amount (default 1) of lines the cursor will scroll in the given
 *    direction.
*/
void esc::scroll(csi direction, word amount) {
   printf("\e[%hhu;%huf", direction + 'S', amount);
}

/* @param row
 *    moves the cursor to the specified row.
 * @param column
 *    moves the cursor to the specified column.
*/
void esc::move_to(word row, word column) {
   // The coordinate system starts at one
   printf("\e[%hu;%huf", row + 1, column + 1);
}

/* @param column
 *    moves the cursor to the specified column.
*/
void esc::move_to(word column) {
   // The coordinate system starts at one
   printf("\e[%huG", column + 1);
}

/* Clear all styles apllied
*/
void esc::reset() {
   printf("\e[0m");
}

/* Set the foreground according to predefinided colors.
 *
 * @param color
 *    BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE or RESET.
 *    BRIGHT_<color> to get a brighter version of the color.
 * @param ground
 *    selects amongst BACKGROUND AND FOREGROUND
*/
void esc::color(clr color, grnd ground) {
   // Backgroung literal are 10 plus foreground ones
   printf("\e[%hhum", ground == BACKGROUND ? color + 10 : color);
}

/* Reset colors to default color (specified by the system).
*/
void esc::color() {
   printf("\e[39;49m");
}

/* Set the specified ground to and 24-bit RGB color
 *
 * @param ground
 *    selects amongst BACKGROUND AND FOREGROUND
 * @param red
 *    especifies the red channel value [0, 255].
 * @param green
 *    especifies the green channel value [0, 255].
 * @param blue
 *    especifies the blue channel value [0, 255].
*/
void esc::color(grnd ground, byte red, byte green, byte blue) {
   printf("\e[%hhu;2;%hhu;%hhu;%hhum", ground, red, green, blue);
}

/* Set the specified ground to and 24-bit RGB color
 *
 * @param ground
 *    selects amongst BACKGROUND AND FOREGROUND
 * @param color
 *    especifies an RGB color 0xRRGGBB.
*/
void esc::color(grnd ground, rgb color) {
   printf("\e[%hhu;2;%lu;%lu;%lum", ground,
      (color & 0xFF0000) >> 16, (color & 0x00FF00) >> 8, (color & 0x0000FF));
}

/* Set or unset bold to the font.
 *
 * @param on true for set and false for unset.
*/
void esc::bold(bool on) {
   printf("\e[%hhum", byte(on ? 1 : 22));
}

/* Set or unset light (faint) to the font.
 *
 * @param on true for set and false for unset.
*/
void esc::light(bool on) {
   printf("\e[%hhum", byte(on ? 2 : 22));
}

/* Set or unset italic to the font.
 *
 * @param on true for set and false for unset.
*/
void esc::italic(bool on) {
   printf("\e[%hhum", byte(on ? 3 : 23));
}

/* Set or unset underline to the font.
 *
 * @param on true for set and false for unset.
*/
void esc::underline(bool on) {
   printf("\e[%hhum", byte(on ? 4 : 24));
}

/* Swap and unswap foreground and background colors
 *
 * @param on true for swap and false for unswap.
*/
void esc::invert(bool on) {
   printf("\e[%hhum", byte(on ? 7 : 27));
}

/* Set or unset strike to the font.
 *
 * @param on true for set and false for unset.
*/
void esc::strike(bool on) {
   printf("\e[%hhum", byte(on ? 9 : 29));
}

/* Set the style of printing
 *
 * @param style
 *    BOLD, LIGHT, ITALIC, UNDERLINE, STRIKE and INVERT, these can be
 *    combined using the bitwise or (|) operator;
 * @param foreground, background
 *    BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE or RESET.
 *    BRIGHT_<color> to get a brighter version of the color.
*/
void esc::style(byte style, clr foreground, clr background) {

   printf("\e[%hhu;%hhu", foreground, 10 + background);

   if (style & BOLD)      printf(";1");
   if (style & LIGHT)     printf(";2");
   if (style & ITALIC)    printf(";3");
   if (style & UNDERLINE) printf(";4");
   if (style & STRIKE)    printf(";9");
   if (style & INVERT)    printf(";7");

   printf("m");
}

/* Set the style of printing
 *
 * @param style
 *    BOLD, LIGHT, ITALIC, UNDERLINE, STRIKE and INVERT, these can be
 *    combined using the bitwise or (|) operator;
 * @param foreground, background
 *    especifies an RGB color 0xRRGGBB.
*/
void esc::style(byte style, rgb foreground, rgb background) {

   printf("\e[0");

   if (background != DEFAULT) printf(";48;2;%lu;%lu;%lu",
      (background & 0xFF0000) >> 16, (background & 0x00FF00) >> 8, (background & 0x0000FF));

   if (foreground != DEFAULT) printf(";38;2;%lu;%lu;%lu",
      (foreground & 0xFF0000) >> 16, (foreground & 0x00FF00) >> 8, (foreground & 0x0000FF));

   if (style & BOLD)      printf(";1");
   if (style & LIGHT)     printf(";2");
   if (style & ITALIC)    printf(";3");
   if (style & UNDERLINE) printf(";4");
   if (style & STRIKE)    printf(";9");
   if (style & INVERT)    printf(";7");

   printf("m");
}
