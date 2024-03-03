#include <data.h>
#include <row_operations.h>
#include <terminal.h>

void editorInsertChar(int c) {
  struct editorConfig *E = getEditorConfig();

  if (E->cy == E->numrows) {
    editorAppendRow("", 0);
  }
  editorRowInsertChar(&E->row[E->cy], E->cx, c);
  E->cx++;
}

void editorDelChar() {
  struct editorConfig *E = getEditorConfig();

  if (E->cy == E->numrows) return;

  erow *row = &E->row[E->cy];
  if (E->cx > 0) {
    editorRowDelChar(row, E->cx - 1);
    E->cx--;
  }
}