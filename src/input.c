#include <settings.h>
#include <stdlib.h>
#include <terminal.h>
#include <unistd.h>

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f) // bitwise AND character with 00011111

/*** functions ***/

void editorMoveCursor(int key) {
  struct editorConfig *E = getEditorConfig();

  switch (key) {
  case ARROW_LEFT:
    if (E->cx != 0) {
      E->cx--;
    }
    break;
  case ARROW_RIGHT:
    if (E->cx != E->screencols - 1) {
      E->cx++;
    }
    break;
  case ARROW_UP:
    if (E->cy != 0) {
      E->cy--;
    }
    break;
  case ARROW_DOWN:
    if (E->cy != E->screenrows - 1) {
      E->cy++;
    }
    break;
  }
}

// Waits for a keypress and handles it
void editorProcessKeypress() {
  int c = editorReadKey();
  struct editorConfig *E = getEditorConfig();

  switch (c) {
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
    E->cx = E->screencols - 1;
    break;

  case PAGE_UP:
  case PAGE_DOWN:
    {
      int times = E->screenrows;
      while (times--)
        // arrow up or arrow down until bottom/top of terminal
        editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
    }
    break;

  case ARROW_UP:
  case ARROW_DOWN:
  case ARROW_LEFT:
  case ARROW_RIGHT:
    editorMoveCursor(c);
    break;
  }
}