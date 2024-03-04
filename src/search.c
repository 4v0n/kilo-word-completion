#include <input.h>
#include <row_operations.h>
#include <stdlib.h>
#include <string.h>
#include <terminal.h>

void editorFindCallback(char *query, int key) {
  if (key == '\r' || key == '\x1b') {
    return;
  }

  struct editorConfig *E = getEditorConfig();

  int i;
  for (i = 0; i < E->numrows; i++) {
    erow *row = &E->row[i];
    char *match = strstr(row->render, query);
    if (match) {
      E->cy = i;
      E->cx = editorRowRxToCx(row, match - row->render);
      break;
    }
  }
}

void editorFind() {
  struct editorConfig *E = getEditorConfig();
  int saved_cx = E->cx;
  int saved_cy = E->cy;
  int saved_coloff = E->coloff;
  int saved_rowoff = E->rowoff;

  char *query = editorPrompt("Search: %s (ESC to cancel)", editorFindCallback);

  if (query) {
    free(query);
  } else {
    E->cx = saved_cx;
    E->cy = saved_cy;
    E->coloff = saved_coloff;
    E->rowoff = saved_rowoff;
  }
}