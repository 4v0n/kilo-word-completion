/*
  This file contains code from antirez's kilo text editor:
  https://github.com/antirez/kilo and was programmed following Paige Ruten's
  "Build Your Own Text Editor" tutorial:
    https://viewsourcecode.org/snaptoken/kilo/index.html |
    https://github.com/snaptoken/kilo-tutorial?tab=readme-ov-file

  This file contains functions that handle output operations
*/

#include <ctype.h>
#include <data.h>
#include <row_operations.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syntax_highlighting.h>
#include <terminal.h>
#include <time.h>
#include <unistd.h>

// empty buffer - constructor for abuf type
#define ABUF_INIT {NULL, 0};

// appends string s to abuf
void abAppend(struct abuf *ab, const char *s, int len) {
  // give us a block of memory that is the size of the current string + size of
  // string we are appending
  char *new = realloc(ab->b, ab->len + len);

  if (new == NULL)
    return;

  // copy the string s after the end of the current data in the buffer
  memcpy(&new[ab->len], s, len);
  ab->b = new;    // update abuf pointer
  ab->len += len; // update length
}

// destructor for abuf
void abFree(struct abuf *ab) {
  free(ab->b); // deallocate pointer
}

/*** output ***/

// scrolls the editor when the cursor goes out of bounds
void editorScroll() {
  struct editorConfig *E = getEditorConfig();

  E->rx = 0; // set render coord to 0
  if (E->cy < E->numrows) {
    // if cursor is within bounds, calculate rx
    E->rx = editorRowCxToRx(&E->row[E->cy], E->cx);
  }

  // vertical
  if (E->cy < E->rowoff) {
    // if cursor is above viewport, mobe into view
    E->rowoff = E->cy;
  }
  if (E->cy >= E->rowoff + E->screenrows) {
    // if cursor is below viewport
    E->rowoff = E->cy - E->screenrows + 1;
  }

  // horizontal
  if (E->rx < E->coloff) {
    E->coloff = E->rx;
  }
  if (E->rx >= E->coloff + E->screencols) {
    E->coloff = E->rx - E->screencols + 1;
  }
}

void displayWelcomeMessage(struct abuf *ab) {
  struct editorConfig *E = getEditorConfig();

  // print editor version on 3rd row
  char welcome[80];
  int welcomelen =
      snprintf(welcome, sizeof(welcome), "Kilo editor -- version %s", VERSION);
  if (welcomelen > E->screencols)
    welcomelen = E->screencols;

  // center message and pad
  int padding = (E->screencols - welcomelen) / 2;
  if (padding) {
    abAppend(ab, "~", 1);
    padding--;
  }
  while (padding--)
    abAppend(ab, " ", 1);

  abAppend(ab, welcome, welcomelen);
}

void drawTextRow(struct abuf *ab, int filerow) {
  struct editorConfig *E = getEditorConfig();

  // calc length of row to be displayed
  int len = E->row[filerow].rsize - E->coloff;
  if (len < 0)
    len = 0;
  if (len > E->screencols)
    len = E->screencols; // adjust for horizontal scroll

  char *c = &E->row[filerow].render[E->coloff]; // render chars
  unsigned char *hl = &E->row[filerow].hl[E->coloff]; // syntax highlight
  int current_colour = -1;

  // loop over each character in row
  int j;
  for (j = 0; j < len; j++) {
    // handle control chars
    if (iscntrl(c[j])) {
      char sym = (c[j] <= 26) ? '@' + c[j] : '?';
      abAppend(ab, "\x1b[7m", 4); // set inverse style
      abAppend(ab, &sym, 1);
      abAppend(ab, "\x1b[m", 3); // turn off inverse

      // reset colour
      if (current_colour != -1) {
        char buf[16];
        int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", current_colour);
        abAppend(ab, buf, clen);
      }

    } else if (hl[j] == HL_NORMAL) {
      // reset colour if current character is not highlighted
      if (current_colour != -1) {
        abAppend(ab, "\x1b[39m", 5);
        current_colour = -1;
      }
      abAppend(ab, &c[j], 1); // append char

    } else { // highlighted character
      int colour = editorSyntaxToColour(hl[j]);
      if (colour != current_colour) {
        current_colour = colour;
        char buf[16];
        int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", colour);
        abAppend(ab, buf, clen);
      }
      abAppend(ab, &c[j], 1);
    }
  }

  // reset colour
  abAppend(ab, "\x1b[39m", 5);
}

// Renders the content of the text editor to the screen
void editorDrawRows(struct abuf *ab) {
  struct editorConfig *E = getEditorConfig();

  // loop through all screen rows
  int y;
  for (y = 0; y < E->screenrows; y++) {
    int filerow = y + E->rowoff;
    if (filerow >= E->numrows) {
      // if empty file and third line
      if (E->numrows == 0 && y == E->screenrows / 3) {
        // no rows in file (empty file)
        displayWelcomeMessage(ab);
      } else {
        // draw tilde on left of screen
        abAppend(ab, "~", 1);
      }
    } else {
      drawTextRow(ab, filerow);
    }

    abAppend(ab, "\x1b[K", 3); // clear line
    abAppend(ab, "\r\n", 2);
  }
}

// Renders the editor's status bar
void editorDrawStatusBar(struct abuf *ab) {
  struct editorConfig *E = getEditorConfig();

  abAppend(ab, "\x1b[7m", 4); // invert colour mode

  char status[80], rstatus[80];

  // left
  int len = snprintf(status, sizeof(status), "%.20s - %d lines %s",
                     E->filename ? E->filename : "[No Name]", E->numrows,
                     E->dirty ? "(modified)" : "");

  // right
  int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d/%d",
                      E->syntax ? E->syntax->filetype : "unknown filetype",
                      E->cy + 1, E->numrows);

  // truncate left if too long
  if (len > E->screencols)
    len = E->screencols;

  abAppend(ab, status, len);

  // fill gap with spaces
  while (len < E->screencols) {
    if (E->screencols - len == rlen) {
      abAppend(ab, rstatus, rlen);
      break;
    } else {
      abAppend(ab, " ", 1); // fill with spaces
      len++;
    }
  }
  abAppend(ab, "\x1b[m", 3); // switch back to normal
  abAppend(ab, "\r\n", 2); // newline
}

// Renders the editor's message bar
void editorDrawMessageBar(struct abuf *ab) {
  struct editorConfig *E = getEditorConfig();

  abAppend(ab, "\x1b[K", 3); // clear row
  int msglen = strlen(E->statusmsg);
  if (msglen > E->screencols) // truncate message length if too long
    msglen = E->screencols;
  if (msglen && time(NULL) - E->statusmsg_time < 5)
    abAppend(ab, E->statusmsg, msglen);
}

// Draws each row of the buffer of text being edited
void editorRefreshScreen() {
  editorScroll();

  struct editorConfig *E = getEditorConfig();
  struct abuf ab = ABUF_INIT; // create new append buffer

  abAppend(&ab, "\x1b[?25l", 6); // hide cursor
  abAppend(&ab, "\x1b[H", 3);    // reposition cursor

  editorDrawRows(&ab);
  editorDrawStatusBar(&ab);
  editorDrawMessageBar(&ab);

  // position cursor
  char buf[32];
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (E->cy - E->rowoff) + 1,
           (E->rx - E->coloff) + 1);
  abAppend(&ab, buf, strlen(buf));

  abAppend(&ab, "\x1b[?25h", 6); // show cursor

  write(STDOUT_FILENO, ab.b, ab.len); // write buffer to standard output
  abFree(&ab);                        // free buffer
}

// Sets the editor's status message
void editorSetStatusMessage(const char *fmt, ...) {
  struct editorConfig *E = getEditorConfig();

  va_list ap;
  va_start(ap, fmt);
  vsnprintf(E->statusmsg, sizeof(E->statusmsg), fmt, ap);
  va_end(ap);
  E->statusmsg_time = time(NULL);
}