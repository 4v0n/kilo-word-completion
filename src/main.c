/*** includes ***/
#include <terminal.h>
#include <input.h>

#include <ctype.h>
#include<stdio.h>
#include <unistd.h>
#include <errno.h>

/*** init ***/
int main() {
  enableRawMode();

  // while the program is running
  while (1) {
    editorProcessKeypress();
  }

  return 0;
}