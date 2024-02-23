/*** includes ***/
#include <input.h>
#include <output.h>
#include <terminal.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

/*** init ***/
int main() {
  enableRawMode();
  initEditor();

  // while the program is running
  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }

  return 0;
}