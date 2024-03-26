/*
  This file contains code from antirez's kilo text editor:
  https://github.com/antirez/kilo and was programmed following Paige Ruten's
  "Build Your Own Text Editor" tutorial:
    https://viewsourcecode.org/snaptoken/kilo/index.html |
    https://github.com/snaptoken/kilo-tutorial?tab=readme-ov-file

  This file handles user input to the text editor
*/

#include <ctype.h>
#include <data.h>
#include <editor_operations.h>
#include <io.h>
#include <output.h>
#include <search.h>
#include <stdlib.h>
#include <terminal.h>
#include <unistd.h>
#include <word_completion.h>

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f) // bitwise AND character with 00011111

/*** functions ***/

// get user input with a prompt
char *editorPrompt(char *prompt, void (*callback)(char *, int)) {
  // Allocate a buffer to store user input
  size_t bufsize = 128;
  char *buf = malloc(bufsize);

  //  initialise buf
  size_t buflen = 0;
  buf[0] = '\0';

  while (1) {
    // display prompt
    editorSetStatusMessage(prompt, buf);
    editorRefreshScreen();

    int c = editorReadKey();
    if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
      // handle special keys
      if (buflen != 0)
        buf[--buflen] = '\0';

    } else if (c == '\x1b') { // esc
      editorSetStatusMessage("");
      if (callback)
        callback(buf, c);
      free(buf);
      return NULL;

    } else if (c == '\r') { // enter
      if (buflen != 0) {
        // end input, call callback and return
        editorSetStatusMessage("");
        if (callback)
          callback(buf, c);
        return buf;
      }
    } else if (!iscntrl(c) && c < 128) { // normal characters
      // append chars to buf
      if (buflen == bufsize - 1) {
        bufsize *= 2;
        buf = realloc(buf, bufsize);
      }
      buf[buflen++] = c;
      buf[buflen] = '\0';
    }

    // call callback with current buf
    if (callback)
      callback(buf, c);
  }
}

// Moves curser based on keyboard input
void editorMoveCursor(int key) {
  struct editorConfig *E = getEditorConfig();

  // get current editing row or set to null if beyond end of file
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

  // bpundary check
  row = (E->cy >= E->numrows) ? NULL : &E->row[E->cy];
  int rowlen = row ? row->size : 0;
  if (E->cx > rowlen) {
    E->cx = rowlen;
  }
}

// Waits for a keypress and handles it
void editorProcessKeypress() {
  static int quit_times = QUIT_TIMES;
  int c = editorReadKey();
  struct editorConfig *E = getEditorConfig();

  switch (c) {

  case '\r':               // enter
    editorInsertNewLine(); // newline
    break;

  case CTRL_KEY('q'): // quit program
    if (E->dirty && quit_times > 0) {
      editorSetStatusMessage(
          "File has unsaved changes! Press Ctrl-Q once more to confirm.");
      quit_times--;
      return;
    }

    // clear screen and reposition cursor on quit
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    exit(0);
    break;

  case CTRL_KEY('s'):
    editorSave();
    break;

  case HOME_KEY:
    E->cx = 0;
    updateEC();
    break;

  case END_KEY:
    // navigate cursor to end of row
    if (E->cy < E->numrows)
      E->cx = E->row[E->cy].size;
    updateEC();
    break;

  case CTRL_KEY('f'):
    editorFind();
    updateEC();
    break;

  case CTRL_KEY('h'):
    editorSetStatusMessage(HELP_MESSAGE);
    break;

  case CTRL_KEY('o'):
    toggleWordCompletionEngine();
    break;

  case CTRL_KEY('\n'):
    completeWord();
    break;

  case CTRL_KEY('a'):
  case CTRL_KEY('d'):
    wordCompletionChooseCompletion(c);
    break;

  case BACKSPACE:
  case DEL_KEY:
    if (c == DEL_KEY)
      editorMoveCursor(ARROW_RIGHT);
    editorDelChar();
    updateEC();
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
    updateEC();
  } break;

  case ARROW_UP:
  case ARROW_DOWN:
  case ARROW_LEFT:
  case ARROW_RIGHT:
    editorMoveCursor(c);
    updateEC();
    break;

  case CTRL_KEY('l'):
  case '\x1b':
    break;

  default:
    editorInsertChar(c);
    updateEC();
    break;
  }

  quit_times = QUIT_TIMES;
}