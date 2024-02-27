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