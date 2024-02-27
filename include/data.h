#ifndef DATA_H
#define DATA_H

#include <termios.h>

// defines
#define VERSION "0.0.1"

// keybinds
enum editorKey {
  ARROW_LEFT = 1000,
  ARROW_RIGHT, // 1001
  ARROW_UP,    // 1002 ...
  ARROW_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN
};

// structs
struct abuf { // append buffer
  char *b;
  int len;
};

typedef struct erow { // editor row
  int size;
  char *chars;
} erow;

struct editorConfig {
  int cx, cy;     // x - horizontal coordinate of cursor, y - vertical
  int rowoff;     // row offset - current row user is scrolled to
  int coloff;     // column offset - current column use is scrolled to
  int screenrows; // no rows in terminal
  int screencols; // no columns in terminal
  int numrows;
  erow *row;
  struct termios orig_termios;
};

#endif