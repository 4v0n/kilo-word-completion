#ifndef TERMINAL_H
#define TERMINAL_H

#include <termios.h>

struct editorConfig {
  int screenrows;
  int screencols;
  struct termios orig_termios;
};

struct editorConfig* getEditorConfig();
void die(const char *s);
void disableRawMode();
void enableRawMode();
char editorReadKey();
int getCursorPosition(int *rows, int *cols);
int getWindowSize(int *rows, int *cols);
void initEditor();

#endif