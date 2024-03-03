#include <data.h>
#include <stdlib.h>
#include <string.h>
#include <terminal.h>

// convert chars index into render index
int editorRowCxToRx(erow *row, int cx) {
  int rx = 0;
  int j;
  for (j = 0; j < cx; j++) {
    if (row->chars[j] == '\t')
      rx += (TAB_STOP - 1) - (rx % TAB_STOP);
    rx++;
  }
  return rx;
}

// fills contents of render string
void editorUpdateRow(erow *row) {
  int tabs = 0;
  int j;
  // loop through chars and count tabs
  for (j = 0; j < row->size; j++)
    if (row->chars[j] == '\t')
      tabs++;

  free(row->render);
  row->render = malloc(row->size + tabs * (TAB_STOP - 1) + 1);

  int idx = 0;
  for (j = 0; j < row->size; j++) {
    if (row->chars[j] == '\t') {
      // if character is tab, append one space
      row->render[idx++] = ' ';
      while (idx % TAB_STOP != 0)
        row->render[idx++] = ' ';
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
  E->dirty++;
}

void editorFreeRow(erow *row) {
  free(row->render);
  free(row->chars);
}

void editorDelRow(int at) {
  struct editorConfig *E = getEditorConfig();

  if (at < 0 || at >= E->numrows) return;
  editorFreeRow(&E->row[at]);
  memmove(&E->row[at], &E->row[at + 1], sizeof(erow) * (E->numrows - at - 1));
  E->numrows--;
  E->dirty++;
}

void editorRowInsertChar(erow *row, int at, int c) {
  if (at < 0 || at > row->size)
    at = row->size;
  row->chars = realloc(row->chars, row->size + 2);
  memmove(&row->chars[at + 1], &row->chars[at], row->size - at + 1);
  row->size++;
  row->chars[at] = c;
  editorUpdateRow(row);
  
  getEditorConfig()->dirty++;
}

void editorRowAppendString(erow *row, char *s, size_t len) {
  row->chars = realloc(row->chars, row->size + len + 1);
  memcpy(&row->chars[row->size], s, len);
  row->size += len;
  row->chars[row->size] = '\0';
  editorUpdateRow(row);
  getEditorConfig()->dirty++;
}

void editorRowDelChar(erow *row, int at) {
  if (at < 0 || at >= row->size) return;
  memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
  row->size--;
  editorUpdateRow(row);
  getEditorConfig()->dirty++;
}