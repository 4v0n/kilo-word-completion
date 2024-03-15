/*
  This file contains code adapted from antirez's kilo text editor:
  https://github.com/antirez/kilo and was programmed following Paige Ruten's
  "Build Your Own Text Editor" tutorial:
    https://viewsourcecode.org/snaptoken/kilo/index.html |
    https://github.com/snaptoken/kilo-tutorial?tab=readme-ov-file

  This file prototypes functions that handle terminal operations
*/

#ifndef TERMINAL_H
#define TERMINAL_H

#include <data.h>

struct editorConfig *getEditorConfig();
void die(const char *s);
void disableRawMode();
void enableRawMode();
int editorReadKey();
int getCursorPosition(int *rows, int *cols);
int getWindowSize(int *rows, int *cols);
void initEditor();

#endif