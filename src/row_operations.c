#include <stdlib.h>
#include <terminal.h>
#include <string.h>
#include <data.h>

// fills contents of render string
void editorUpdateRow(erow *row) {
  int tabs = 0;
  int j;
  // loop through chars and count tabs
  for (j = 0; j < row->size; j++)
    if (row->chars[j] == '\t') tabs++;

  free(row->render);
  row->render = malloc(row->size + tabs*(TAB_STOP - 1) + 1);

  int idx = 0;
  for (j = 0; j < row->size; j++) {
    if (row->chars[j] == '\t') {
      // if character is tab, append one space
      row->render[idx++] = ' ';
      while (idx % TAB_STOP != 0) row->render[idx++] = ' ';
    } else {
      row->render[idx++] = row->chars[j];
    }
  }
  row->render[idx] = '\0';
  row->rsize = idx;
}

void editorAppendRow(char *s, size_t len) {
  struct editorConfig *E = getEditorConfig();

  E->row = realloc(E->row, sizeof(erow) * (E->numrows + 1));

  int at = E->numrows;
  E->row[at].size = len;
  E->row[at].chars = malloc(len + 1);
  memcpy(E->row[at].chars, s, len);
  E->row[at].chars[len] = '\0';

  // initialise render
  E->row[at].rsize = 0;
  E->row[at].render = NULL;
  editorUpdateRow(&E->row[at]);

  E->numrows++;
}