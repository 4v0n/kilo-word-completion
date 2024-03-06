/*
  This file contains code from antirez's kilo text editor:
  https://github.com/antirez/kilo and was programmed following Paige Ruten's
  "Build Your Own Text Editor" tutorial:
    https://viewsourcecode.org/snaptoken/kilo/index.html |
    https://github.com/snaptoken/kilo-tutorial?tab=readme-ov-file

  This file handles editing operations, such as inserting and deleting characters, and inserting new lines.
  This code relates to the "editor operations" section
*/

#include <data.h>
#include <row_operations.h>
#include <terminal.h>

// inserts a character at the current cursor position
void editorInsertChar(int c) {
  struct editorConfig *E = getEditorConfig();

  // if cursor is at the end of the last row
  if (E->cy == E->numrows) {
    editorInsertRow(E->numrows, "", 0); // create new empty row
  }

  /*
    insert character
    &E->row[E->cy] - the current row
    E->cx - the current cursor position
    c - char
  */
  editorRowInsertChar(&E->row[E->cy], E->cx, c);
  E->cx++;
}

/*
  Inserts a new line, splitting current line and moving cursor to beginning of
  next line
*/
void editorInsertNewLine() {
  struct editorConfig *E = getEditorConfig();

  if (E->cx == 0) {                // if cursor is at beginning of line
    editorInsertRow(E->cy, "", 0); // create new blank row above current one
  } else {
    erow *row = &E->row[E->cy];
    // insert new row below current one
    editorInsertRow(
        E->cy + 1 /*below*/, &row->chars[E->cx] /*text after cursor pos*/,
        row->size - E->cx /*size of current row - text before cursor pos*/);
    row = &E->row[E->cy];
    row->chars[row->size] = '\0';
    editorUpdateRow(row); // render row
  }
  // move cursor
  E->cy++;
  E->cx = 0;
}

/*
  Deletes the character at the current cursor position, merging lines if at the
  beginning of a line
*/
void editorDelChar() {
  struct editorConfig *E = getEditorConfig();

  // returns of the editor
  if (E->cy == E->numrows) // editor is empty
    return;
  if (E->cx == 0 && E->cy == 0) // if cursor at very beginning
    return;

  erow *row = &E->row[E->cy];
  if (E->cx > 0) {                    // cursor not at beginning of line
    editorRowDelChar(row, E->cx - 1); // delete character before cursor
    E->cx--;                          // move cursor back
  } else {                            // cursor at beginning of line
    E->cx = E->row[E->cy - 1].size;   // move to end of prev line
    editorRowAppendString(&E->row[E->cy - 1], row->chars,
                          row->size); // merge row
    editorDelRow(E->cy);              // delete initial row
    E->cy--;                          // move cursor up
  }
}