#ifndef ROW_OPERATIONS_H
#define ROW_OPERATIONS_H

#include <data.h>
#include <stdlib.h>

int editorRowCxToRx(erow *row, int cx);
void editorUpdateRow(erow *row);
void editorAppendRow(char *s, size_t len);

#endif