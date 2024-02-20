/*** includes ***/
#include <terminal.h>
#include <input.h>
#include <output.h>

#include <ctype.h>
#include<stdio.h>
#include <unistd.h>
#include <errno.h>

/*** init ***/
int main() {
  enableRawMode();

  // while the program is running
  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }

  return 0;
}