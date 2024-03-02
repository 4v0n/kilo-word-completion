#include <data.h>
#include <editor_operations.h>
#include <stdlib.h>
#include <terminal.h>
#include <unistd.h>

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f) // bitwise AND character with 00011111

/*** functions ***/

void editorMoveCursor(int key) {
  struct editorConfig *E = getEditorConfig();

  erow *row = (E->cy >= E->numrows) ? NULL : &E->row[E->cy];

  switch (key) {
  case ARROW_LEFT:
    if (E->cx != 0) {
      E->cx--;
    } else if (E->cy > 0) {
      E->cy--;
      E->cx = E->row[E->cy].size;
    }
    break;
  case ARROW_RIGHT:
    if (row && E->cx < row->size) {
      E->cx++;
    } else if (row && E->cx == row->size) {
      E->cy++;
      E->cx = 0;
    }
    break;
  case ARROW_UP:
    if (E->cy != 0) {
      E->cy--;
    }
    break;
  case ARROW_DOWN:
    if (E->cy < E->numrows) {
      E->cy++;
    }
    break;
  }

  row = (E->cy >= E->numrows) ? NULL : &E->row[E->cy];
  int rowlen = row ? row->size : 0;
  if (E->cx > rowlen) {
    E->cx = rowlen;
  }
}

// Waits for a keypress and handles it
void editorProcessKeypress() {
  int c = editorReadKey();
  struct editorConfig *E = getEditorConfig();

  switch (c) {

  case '\r':
    // todo
    break;

  case CTRL_KEY('q'): // quit program
    // clear screen and reposition cursor on quit
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    exit(0);
    break;

  case HOME_KEY:
    E->cx = 0;
    break;

  case END_KEY:
    if (E->cy < E->numrows)
      E->cx = E->row[E->cy].size;
    break;

  case BACKSPACE:
  case CTRL_KEY('h'):
  case DEL_KEY:
    // todo
    break;

  case PAGE_UP:
  case PAGE_DOWN: {
    if (c == PAGE_UP) {
      E->cy = E->rowoff;
    } else if (c == PAGE_DOWN) {
      E->cy = E->rowoff + E->screenrows - 1;
      if (E->cy > E->numrows)
        E->cy = E->numrows;
    }

    int times = E->screenrows;
    while (times--)
      // arrow up or arrow down until bottom/top of terminal
      editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
  } break;

  case ARROW_UP:
  case ARROW_DOWN:
  case ARROW_LEFT:
  case ARROW_RIGHT:
    editorMoveCursor(c);
    break;

  case CTRL_KEY('l'):
  case '\x1b':
    break;

  default:
    editorInsertChar(c);
    break;
  }
}