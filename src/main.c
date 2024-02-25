/*** includes ***/
#include <input.h>
#include <io.h>
#include <output.h>
#include <terminal.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

/*** init ***/
int main(int argc, char *argv[]) {
  enableRawMode();
  initEditor();
  if (argc >= 2) {
    editorOpen(argv[1]);
  }

  // while the program is running
  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }

  return 0;
}