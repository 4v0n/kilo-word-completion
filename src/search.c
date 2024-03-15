/*
  This file contains code from antirez's kilo text editor:
  https://github.com/antirez/kilo and was programmed following Paige Ruten's
  "Build Your Own Text Editor" tutorial:
    https://viewsourcecode.org/snaptoken/kilo/index.html |
    https://github.com/snaptoken/kilo-tutorial?tab=readme-ov-file

  This file handles search operations
*/

#include <input.h>
#include <row_operations.h>
#include <stdlib.h>
#include <string.h>
#include <terminal.h>

void editorFindCallback(char *query, int key) {
  struct editorConfig *E = getEditorConfig();

  // search state variables
  static int last_match = -1;
  static int direction = 1;

  // current line highlight state
  static int saved_hl_line;
  static char *saved_hl = NULL;

  // restore highlight before continuing
  if (saved_hl) {
    memcpy(E->row[saved_hl_line].hl, saved_hl, E->row[saved_hl_line].rsize);
    free(saved_hl);
    saved_hl = NULL;
  }

  // Handle key inputs
  if (key == '\r' || key == '\x1b') {
    last_match = -1;
    direction = 1;
    return;
  } else if (key == ARROW_RIGHT || key == ARROW_DOWN) {
    direction = 1;
  } else if (key == ARROW_LEFT || key == ARROW_UP) {
    direction = -1;
  } else {
    last_match = -1;
    direction = 1;
  }

  // reset direction if starting new search
  if (last_match == -1) direction = 1;

  int current = last_match; // start from last match
  
  // loop through all rows in specified direction
  for (int i = 0; i < E->numrows; i++) {
    current += direction;
    if (current == -1) current = E->numrows - 1;
    else if (current == E->numrows) current = 0;

    // Query current row
    erow *row = &E->row[current];
    char *match = strstr(row->render, query);
    if (match) {
      last_match = current;

      // move cursor to the match
      E->cy = current;
      E->cx = editorRowRxToCx(row, match - row->render);
      E->rowoff = E->numrows;

      // save line highlight state
      saved_hl_line = current;
      saved_hl = malloc(row->rsize);
      memcpy(saved_hl, row->hl, row->rsize);
      memset(&row->hl[match - row->render], HL_MATCH, strlen(query));
      break;
    }
  }
}

// Interactively search text within the editor
void editorFind() {
  struct editorConfig *E = getEditorConfig();

  // save current cursor position
  int saved_cx = E->cx;
  int saved_cy = E->cy;
  int saved_coloff = E->coloff;
  int saved_rowoff = E->rowoff;

  // prompt user
  char *query = editorPrompt("Search: %s (ESC to cancel | Navigate with arrows)", editorFindCallback);

  if (query) {
    free(query);
  } else {
    // reset cursor position
    E->cx = saved_cx;
    E->cy = saved_cy;
    E->coloff = saved_coloff;
    E->rowoff = saved_rowoff;
  }
}