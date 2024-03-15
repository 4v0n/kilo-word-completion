/*
  This file contains code from antirez's kilo text editor:
  https://github.com/antirez/kilo and was programmed following Paige Ruten's
  "Build Your Own Text Editor" tutorial:
    https://viewsourcecode.org/snaptoken/kilo/index.html |
    https://github.com/snaptoken/kilo-tutorial?tab=readme-ov-file

  This file handles editor row operations
*/

#include <data.h>
#include <stdlib.h>
#include <string.h>
#include <syntax_highlighting.h>
#include <terminal.h>

// convert chars index into render index
int editorRowCxToRx(erow *row, int cx) {
  int rx = 0;

  // loop through each char in the row until cursor
  int j;
  for (j = 0; j < cx; j++) {
    if (row->chars[j] == '\t') // check is tab
      // if tab, add number of spaces to tab stop
      rx += (TAB_STOP - 1) - (rx % TAB_STOP);
    rx++;
  }
  return rx;
}

// convert render index to char index
int editorRowRxToCx(erow *row, int rx) {
  int cur_rx = 0;

  // loop through each char in row
  int cx;
  for (cx = 0; cx < row->size; cx++) {
    if (row->chars[cx] == '\t') // check is tab
      cur_rx += (TAB_STOP - 1) - (cur_rx % TAB_STOP);
    cur_rx++;

    if (cur_rx > rx)
      return cx;
  }
  return cx;
}

// updates rendered editor row
void editorUpdateRow(erow *row) {
  int tabs = 0;
  int j;
  // loop through chars and count tabs
  for (j = 0; j < row->size; j++)
    if (row->chars[j] == '\t')
      tabs++;

  // allocate memory for rendered content
  free(row->render);
  row->render = malloc(row->size + tabs * (TAB_STOP - 1) + 1);

  int idx = 0; // index for inserting chars into render array
  for (j = 0; j < row->size; j++) { // over each char
    if (row->chars[j] == '\t') {
      // if character is tab, append one space
      row->render[idx++] = ' ';

      // insert spaces until next tab stop
      while (idx % TAB_STOP != 0)
        row->render[idx++] = ' ';

    } else {
      // copy directly into rendered text
      row->render[idx++] = row->chars[j];
    }
  }

  row->render[idx] = '\0'; // Null terminate rendered string
  row->rsize = idx;        // Update rendered size

  // appy syntax highlighting
  editorUpdateSyntax(row);
}

// insert a new row into the editor at a specified position
void editorInsertRow(int at, char *s, size_t len) {
  struct editorConfig *E = getEditorConfig();

  // if out of bounds, abort
  if (at < 0 || at > E->numrows)
    return;

  // reallocate row array to accomadate one more erow
  E->row = realloc(E->row, sizeof(erow) * (E->numrows + 1));
  memmove(&E->row[at + 1], &E->row[at], sizeof(erow) * (E->numrows - at));

  // upddate index of rows that have been moved down
  for (int j = at + 1; j <= E->numrows; j++)
    E->row[j].idx++;

  // Initialise new row index and size
  E->row[at].idx = at;
  E->row[at].size = len;

  // Allocate memory for row characters and copy provided text
  E->row[at].chars = malloc(len + 1);
  memcpy(E->row[at].chars, s, len);
  E->row[at].chars[len] = '\0';

  // initialise row properties
  E->row[at].rsize = 0;
  E->row[at].render = NULL;
  E->row[at].hl = NULL;
  E->row[at].hl_open_comment = 0;

  editorUpdateRow(&E->row[at]);

  E->numrows++;
  E->dirty++;
}

// Free memory allocated for row rendering, chars and syntax highlighting
void editorFreeRow(erow *row) {
  free(row->render);
  free(row->chars);
  free(row->hl);
}

// Deletes a row at the specified index
void editorDelRow(int at) {
  struct editorConfig *E = getEditorConfig();

  // If out of bounds, abort
  if (at < 0 || at >= E->numrows)
    return;

  editorFreeRow(&E->row[at]); // free memory of the row to be deleted

  // shift remaining row up
  memmove(&E->row[at], &E->row[at + 1], sizeof(erow) * (E->numrows - at - 1));

  // update row indexes
  for (int j = at; j < E->numrows - 1; j++)
    E->row[j].idx++;

  E->numrows--;
  E->dirty++;
}

// Inserts a character at the specified position in a row
void editorRowInsertChar(erow *row, int at, int c) {

  // if out of bounds, insert to end of row
  if (at < 0 || at > row->size)
    at = row->size;

  row->chars = realloc(row->chars, row->size + 2); // resize array
  memmove(&row->chars[at + 1], &row->chars[at],
          row->size - at + 1); // shift chars right

  row->size++;
  row->chars[at] = c;

  editorUpdateRow(row);

  getEditorConfig()->dirty++;
}

// Appends a string to the end of a row
void editorRowAppendString(erow *row, char *s, size_t len) {
  row->chars = realloc(row->chars, row->size + len + 1);
  memcpy(&row->chars[row->size], s, len);
  row->size += len;
  row->chars[row->size] = '\0';
  editorUpdateRow(row);
  getEditorConfig()->dirty++;
}

// Deletes a char from a row at the specified position
void editorRowDelChar(erow *row, int at) {
  if (at < 0 || at >= row->size)
    return;
    
  memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
  row->size--;
  editorUpdateRow(row);
  getEditorConfig()->dirty++;
}