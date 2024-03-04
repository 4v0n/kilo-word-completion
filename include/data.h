#ifndef DATA_H
#define DATA_H

#include <termios.h>
#include <time.h>

// defines
#define VERSION "0.0.1"
#define TAB_STOP 8
#define QUIT_TIMES 1
#define HELP_MESSAGE                                                           \
  "HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find | Ctrl-H = show help"

// keybinds
enum editorKey {
  BACKSPACE = 127,
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

// syntax highlighting
enum editorHighlight {
  HL_NORMAL = 0,
  HL_NUMBER
};

// structs
struct abuf { // append buffer
  char *b;
  int len;
};

typedef struct erow { // editor row
  int size;
  int rsize;
  char *chars;
  char *render;
  unsigned char *hl;
} erow;

struct editorConfig {
  int cx, cy;     // x - index to char field of erow, y - vertical coord
  int rx;         // horizontal coordinate
  int rowoff;     // row offset - current row user is scrolled to
  int coloff;     // column offset - current column use is scrolled to
  int screenrows; // no rows in terminal
  int screencols; // no columns in terminal
  int numrows;    // number of rows in the editor
  erow *row;      // row being edited
  int dirty;      // whether file is changed
  char *filename;
  char statusmsg[80];    // editor status message
  time_t statusmsg_time; // amount of time status message is visible
  struct termios orig_termios;
};

#endif