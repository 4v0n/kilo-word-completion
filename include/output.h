#ifndef OUTPUT_H
#define OUTPUT_H

#include <data.h>

// append buffer
void abAppend(struct abuf *ab, const char *s, int len);
void abFree(struct abuf *ab);

// output
void editorDrawRows(struct abuf *ab);
void editorRefreshScreen();

#endif