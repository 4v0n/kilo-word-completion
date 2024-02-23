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
    E->cx--;
    break;
  case ARROW_RIGHT:
    E->cx++;
    break;
  case ARROW_UP:
    E->cy--;
    break;
  case ARROW_DOWN:
    E->cy++;
    break;
  }
}

// Waits for a keypress and handles it
void editorProcessKeypress() {
  int c = editorReadKey();

  switch (c) {
  case CTRL_KEY('q'): // quit program
    // clear screen and reposition cursor on quit
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    exit(0);
    break;

  case ARROW_UP:
  case ARROW_DOWN:
  case ARROW_LEFT:
  case ARROW_RIGHT:
    editorMoveCursor(c);
    break;
  }
}