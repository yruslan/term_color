#ifndef _COLOR_H_INCLUDED
#define _COLOR_H_INCLUDED

#define CN_NO_COLOR    -1

// Colors
#define CN_BLACK       0
#define CN_RED         1
#define CN_GREEN       2
#define CN_YELLOW      3
#define CN_BLUE        4
#define CN_MAGENTA     5
#define CN_CYAN        6
#define CN_WHITE       7

// Color modifier
#define CN_BRIGHT      16

// Character attributes
#define CN_NORMAL      0
#define CN_REVERSE     128

// Character attributes (Linux only)
#define CN_BOLD        32
#define CN_UNDERLINE   64

bool is_inside_terminal();

void textcolor(int attr=CN_NORMAL, int fg=CN_NO_COLOR, int bg=CN_NO_COLOR);

void textcolor_show();

#endif //_COLOR_H_INCLUDED
