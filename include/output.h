/*
  This file contains code adapted from antirez's kilo text editor:
  https://github.com/antirez/kilo and was programmed following Paige Ruten's
  "Build Your Own Text Editor" tutorial:
    https://viewsourcecode.org/snaptoken/kilo/index.html |
    https://github.com/snaptoken/kilo-tutorial?tab=readme-ov-file

  This file prototypes functions that handle output operations
*/

#ifndef OUTPUT_H
#define OUTPUT_H

#include <data.h>

// append buffer
void abAppend(struct abuf *ab, const char *s, int len);
void abFree(struct abuf *ab);

// output
void editorScroll();
void editorDrawRows(struct abuf *ab);
void editorDrawStatusBar(struct abuf *ab);
void editorDrawMessageBar(struct abuf *ab);
void editorRefreshScreen();
void editorSetStatusMessage(const char *fmt, ...);

#endif