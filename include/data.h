/*
  This file contains code adapted from antirez's kilo text editor:
  https://github.com/antirez/kilo and was programmed following Paige Ruten's
  "Build Your Own Text Editor" tutorial:
    https://viewsourcecode.org/snaptoken/kilo/index.html |
    https://github.com/snaptoken/kilo-tutorial?tab=readme-ov-file

  This file defines global structures and values
*/

#ifndef DATA_H
#define DATA_H

#include <termios.h>
#include <time.h>

/* defines */

#define VERSION "0.0.1"
#define TAB_STOP 8
#define QUIT_TIMES 1
#define HELP_MESSAGE                                                           \
  "HELP: Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find | Ctrl-H = show help"

// syntax highlighting bit flags
#define HL_HIGHLIGHT_NUMBERS (1 << 0)
#define HL_HIGHLIGHT_STRINGS (1 << 1)

/* enums */

// colour codes
enum foregroundColours {
  BLACK = 30,
  RED,
  GREEN,
  YELLOW,
  BLUE,
  MAGENTA,
  CYAN,
  WHITE
};

enum backgroundColours {
  BLACK_BG = 40,
  RED_BG,
  GREEN_BG,
  YELLOW_BG,
  BLUE_BG,
  MAGENTA_BG,
  CYAN_BG,
  WHITE_BG
};

// keybind keycodes
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

// highlighting types
enum editorHighlight {
  HL_NORMAL = 0,
  HL_COMMENT,
  HL_MLCOMMENT,
  HL_KEYWORD1,
  HL_KEYWORD2,
  HL_STRING,
  HL_NUMBER,
  HL_MATCH
};

/* structs  */

// details about file type
struct editorSyntax {
  int id;
  char *filetype; // name of filetype
  char **
      filematch; // array of strings, each a pattern to match a filename against
  char **keywords; // arrays of strings of keywords
  char *singleline_comment_start;
  char *multiline_comment_start;
  char *multiline_comment_end;
  int flags; // bit field containing flags whether to highlight numbers and
             // whether to highlight strings for filetype
};

// append buffer for string manipulation
struct abuf {
  char *b; // string
  int len; // length (size) of string
};

// Represents a row of text in the editor
typedef struct erow {
  int idx;           // index of row in editor
  int size;          // number of characters the string is (size/len)
  int rsize;         // Allocated space for the render field
  char *chars;       // text content of the row
  char *render;      // stores text as displayed on screen
  unsigned char *hl; // array where each character corresponds to a character in
                     // *render - for highlighting
  int hl_open_comment; // bit flag - is in multiline comment?
} erow;

struct editorConfig {
  int cx;         // X coord of cursor
  int cy;         // Y coord of cursor (0 = top)
  int rx;         // "real" horizontal coordinate
  int rowoff;     // row offset - current row user is scrolled to
  int coloff;     // column offset - current column use is scrolled to
  int screenrows; // no rows in terminal
  int screencols; // no columns in terminal
  int numrows;    // number of rows in the editor
  erow *row;      // row being edited - access with [cy]
  int dirty;      // whether file is changed
  char *filename;
  char statusmsg[80];    // editor status message
  time_t statusmsg_time; // amount of time status message is visible
  struct editorSyntax *syntax;
  struct termios orig_termios;
};

#endif