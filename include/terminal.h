#ifndef TERMINAL_H
#define TERMINAL_H

#include <termios.h>

typedef struct erow { // editor row
  int size;
  char *chars;
} erow;

struct editorConfig {
  int cx, cy;     // x - horizontal coordinate of cursor, y - vertical
  int screenrows; // no rows in terminal
  int screencols; // no columns in terminal
  int numrows;
  erow row;
  struct termios orig_termios;
};

struct editorConfig *getEditorConfig();
void die(const char *s);
void disableRawMode();
void enableRawMode();
int editorReadKey();
int getCursorPosition(int *rows, int *cols);
int getWindowSize(int *rows, int *cols);
void initEditor();

#endif