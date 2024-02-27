#include <settings.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <terminal.h>
#include <unistd.h>

/*** append buffer ***/

struct abuf {
  char *b; // buffer
  int len; // length
};

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

// Draw a column of tildes on the left side of the screen
void editorDrawRows(struct abuf *ab) {
  int y;
  struct editorConfig *E = getEditorConfig();

  for (y = 0; y < E->screenrows; y++) {
    if (y >= E->numrows) {
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
      int len = E->row[y].size;
      if (len > E->screencols)
        len = E->screencols;
      abAppend(ab, E->row[y].chars, len);
    }

    abAppend(ab, "\x1b[K", 3); // clear line
    if (y < E->screenrows - 1) {
      abAppend(ab, "\r\n", 2);
    }
  }
}

// Draws each row of the buffer of text being edited
void editorRefreshScreen() {
  struct editorConfig *E = getEditorConfig();
  struct abuf ab = ABUF_INIT; // create new append buffer

  abAppend(&ab, "\x1b[?25l", 6); // hide cursor
  abAppend(&ab, "\x1b[H", 3);    // reposition cursor

  editorDrawRows(&ab);

  // position cursor
  char buf[32];
  snprintf(buf, sizeof(buf), "\x1b[%d;%dH", E->cy + 1, E->cx + 1);
  abAppend(&ab, buf, strlen(buf));

  abAppend(&ab, "\x1b[?25h", 6); // show cursor

  write(STDOUT_FILENO, ab.b, ab.len); // write buffer to standard output
  abFree(&ab);                        // free buffer
}
