#include <input.h>
#include <row_operations.h>
#include <stdlib.h>
#include <string.h>
#include <terminal.h>

void editorFind() {
  char *query = editorPrompt("Search: %s (ESC to cancel)");
  if (query == NULL)
    return;

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

  free(query);
}