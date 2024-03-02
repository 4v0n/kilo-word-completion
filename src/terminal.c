/*** includes ***/
#include <data.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

struct editorConfig E;

/*** functions ***/

struct editorConfig *getEditorConfig() { return &E; }

// Prints an error message and exits the program
void die(const char *s) {
  // clear screen and reposition cursor on exit
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(s); // prints a descriptive error message
  exit(1);   // exit program with status 1 - indicate failure
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &E.orig_termios) ==
      -1) // read current terminal attributes into struct
    die("tcgetattr");
  atexit(
      disableRawMode); // register disableRawMode() to be called on program exit

  /*
    disable terminal features

    &= bitwise AND and assignment, ~ bitwise NOT

    c_lflag - local flags (miscellaneous flags)
    c_iflag - input flags
    c_oflag - output flags
    c_cflag - config flags
  */
  struct termios raw =
      E.orig_termios; // make copy of original terminal attributes

  /*
    BRKINT - a break condition will cause a terminate signal to be sent to the
    program ICRNL - translates carriage returns inputted by the user into
    newlines (into 10) INPCK - enables parity checking ISTRIP - causes the 8th
    bit of each input byte to be stripped IXON - allows stopping of transmission
    of data to terminal and allow resuming with ctrl+s and ctrl+q
  */
  raw.c_iflag &=
      ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); // disable above features

  /*
    OPOST - output processing
  */
  raw.c_oflag &= ~(OPOST); // disable above features

  /*
    CS8 - sets the character size to 8 bits per byte
  */
  raw.c_lflag |= (CS8); // enable above features

  /*
    ECHO - causes each key typed to be printed to terminal
    ICANON - causes inputs to be read line by line, disabling it makes it read
    byte by byte IEXTEN - terminal waits for another character after ctrl+v ISIG
    - allows process to be terminated with ctrl+c (SIGINT) and suspended with
    ctrl+z (SIGTSTP)
  */
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // disable above features

  /*
  c_cc - control characters
  VMIN - minimum number of bytes of input needed before read() can return
  VTIME - maximum amount of time to wait before read() returns in (1/10s)
  */
  raw.c_cc[VMIN] = 0; // set VMIN to 0 so read() returns as soon as there is any
                      // input to be read
  raw.c_cc[VTIME] = 1; // set VTIME to 1 to make read return every 100ms (10Hz)

  /*
    write the new terminal attributes

    TCSCAFLUSH specifies when to apply the change
      waits for all pending output to be written to the terminal
      also discards any input that hasn't been read
  */
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
}

// Waits for a key input and returns it
int editorReadKey() {
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
    if (nread == -1 && errno != EAGAIN)
      die("read");
  }

  // if the key is an escape sequence (control key)
  if (c == '\x1b') {
    char seq[3];

    if (read(STDIN_FILENO, &seq[0], 1) != 1)
      return '\x1b';
    if (read(STDIN_FILENO, &seq[1], 1) != 1)
      return '\x1b';

    if (seq[0] == '[') {
      if (seq[1] >= '0' && seq[1] <= '9') {
        if (read(STDIN_FILENO, &seq[2], 1) != 1)
          return '\x1b';
        if (seq[2] == '~') {
          switch (seq[1]) {
          case '1':
            return HOME_KEY;
          case '3':
            return DEL_KEY;
          case '4':
            return END_KEY;
          case '5':
            return PAGE_UP;
          case '6':
            return PAGE_DOWN;
          case '7':
            return HOME_KEY;
          case '8':
            return END_KEY;
          }
        }
      } else {
        switch (seq[1]) {
        case 'A':
          return ARROW_UP;
        case 'B':
          return ARROW_DOWN;
        case 'C':
          return ARROW_RIGHT;
        case 'D':
          return ARROW_LEFT;
        case 'H':
          return HOME_KEY;
        case 'F':
          return END_KEY;
        }
      }
    } else if (seq[0] == '0') {
      switch (seq[1]) {
      case 'H':
        return HOME_KEY;
      case 'F':
        return END_KEY;
      }
    }

    return '\x1b';
  } else {
    return c;
  }
}

// returns the position of the cursor
int getCursorPosition(int *rows, int *cols) {
  char buf[32];
  unsigned int i = 0;
  if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
    return -1;
  while (i < sizeof(buf) - 1) {
    if (read(STDIN_FILENO, &buf[i], 1) != 1)
      break;
    if (buf[i] == 'R')
      break;
    i++;
  }
  buf[i] = '\0';
  if (buf[0] != '\x1b' || buf[1] != '[')
    return -1;
  if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
    return -1;
  return 0;
}

int getWindowSize(int *rows, int *cols) {
  struct winsize ws;

  // ioctl reads the number of columns and rows in the terminal into the ws
  // struct
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    // position cursor at bottom right, then read position to get columns and
    // rows
    if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
      return -1;
    return getCursorPosition(rows, cols);
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

// Initialise fields of the global editorConfig
void initEditor() {
  E.cx = 0;
  E.cy = 0;
  E.rx = 0;
  E.rowoff = 0;
  E.coloff = 0;
  E.numrows = 0;
  E.row = NULL;
  E.filename = NULL;

  if (getWindowSize(&E.screenrows, &E.screencols) == -1)
    die("getWindowSize");
  E.screenrows -= 1;
}