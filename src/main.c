/*
  This file contains code from antirez's kilo text editor:
  https://github.com/antirez/kilo and was programmed following Paige Ruten's
  "Build Your Own Text Editor" tutorial:
    https://viewsourcecode.org/snaptoken/kilo/index.html |
    https://github.com/snaptoken/kilo-tutorial?tab=readme-ov-file

  This file handles file provides the main function and initialises the editor
*/

/*** includes ***/
#include <input.h>
#include <io.h>
#include <output.h>
#include <terminal.h>
#include <word_completion.h>

#include <ctype.h>
#include <data.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

/*** init ***/
int main(int argc, char *argv[]) {
  enableRawMode();
  initEditor();
  initWordCompletionEngine();

  if (argc >= 2) {
    editorOpen(argv[1]);
  }

  editorSetStatusMessage(HELP_MESSAGE);

  // while the program is running
  while (1) {
    editorRefreshScreen();
    editorProcessKeypress();
  }

  return 0;
}