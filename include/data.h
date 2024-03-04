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

#define HL_HIGHLIGHT_NUMBERS (1 << 0)
#define HL_HIGHLIGHT_STRINGS (1 << 1)

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

// highlighting
enum editorHighlight {
  HL_NORMAL = 0,
  HL_COMMENT,
  HL_STRING,
  HL_NUMBER,
  HL_MATCH
};

// structs
struct editorSyntax {
  char *filetype; // name of filetype
  char **
      filematch; // array of strings, each a pattern to match a filename against
  char *singleline_comment_start;
  int flags;     // bit field containing flags whether to highlight numbers and
                 // whether to highlight strings for filetype
};

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
  struct editorSyntax *syntax;
  struct termios orig_termios;
};

#endif