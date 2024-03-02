#include <data.h>
#include <row_operations.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

  E->rx = 0;
  if (E->cy < E->numrows) {
    E->rx = editorRowCxToRx(&E->row[E->cy], E->cx);
  }

  // vertical
  if (E->cy < E->rowoff) {
    E->rowoff = E->cy;
  }
  if (E->cy >= E->rowoff + E->screenrows) {
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

// Draw a column of tildes on the left side of the screen
void editorDrawRows(struct abuf *ab) {
  int y;
  struct editorConfig *E = getEditorConfig();

  for (y = 0; y < E->screenrows; y++) {
    int filerow = y + E->rowoff;
    if (filerow >= E->numrows) {
      if (E->numrows == 0 && y == E->screenrows / 3) {
        // print editor version on 3rd row
        char welcome[80];
        int welcomelen = snprintf(welcome, sizeof(welcome),
                                  "Kilo editor -- version %s", VERSION);
        if (welcomelen > E->screencols)
          welcomelen = E->screencols;
        int padding = (E->screencols - welcomelen) / 2;
        if (padding) {
          abAppend(ab, "~", 1);
          padding--;
        }
        while (padding--)
          abAppend(ab, " ", 1);
        abAppend(ab, welcome, welcomelen);
      } else {
        abAppend(ab, "~", 1);
      }
    } else {
      int len = E->row[filerow].rsize - E->coloff;
      if (len < 0)
        len = 0;
      if (len > E->screencols)
        len = E->screencols;
      abAppend(ab, &E->row[filerow].render[E->coloff], len);
    }

    abAppend(ab, "\x1b[K", 3); // clear line
    abAppend(ab, "\r\n", 2);
  }
}

void editorDrawStatusBar(struct abuf *ab) {
  struct editorConfig *E = getEditorConfig();

  abAppend(ab, "\x1b[7m", 4); // <esc>[7m switches to inverted colours

  char status[80], rstatus[80];
  int len = snprintf(status, sizeof(status), "%.20s - %d lines",
                     E->filename ? E->filename : "[No Name]", E->numrows);
  int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d", E->cy + 1, E->numrows);

  if (len > E->screencols)
    len = E->screencols;
  abAppend(ab, status, len);

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
  abAppend(ab, "\r\n", 2);
}

void editorDrawMessageBar(struct abuf *ab) {
  struct editorConfig *E = getEditorConfig();

  abAppend(ab, "\x1b[K", 3);
  int msglen = strlen(E->statusmsg);
  if (msglen > E->screencols)
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

void editorSetStatusMessage(const char *fmt, ...) {
  struct editorConfig *E = getEditorConfig();

  va_list ap;
  va_start(ap, fmt);
  vsnprintf(E->statusmsg, sizeof(E->statusmsg), fmt, ap);
  va_end(ap);
  E->statusmsg_time = time(NULL);
}