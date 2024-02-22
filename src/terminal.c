/*** includes ***/
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>

/*** data ***/
struct editorConfig {
  int screenrows;
  int screencols;
  struct termios orig_termios;
};

struct editorConfig E;

/*** functions ***/

struct editorConfig* getEditorConfig() {
  return &E;
}

// Prints an error message and exits the program
void die(const char *s)
{
  // clear screen and reposition cursor on exit
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);

  perror(s); // prints a descriptive error message
  exit(1);   // exit program with status 1 - indicate failure
}

void disableRawMode()
{
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode()
{
  if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) // read current terminal attributes into struct
    die("tcgetattr");
  atexit(disableRawMode); // register disableRawMode() to be called on program exit

  /*
    disable terminal features

    &= bitwise AND and assignment, ~ bitwise NOT

    c_lflag - local flags (miscellaneous flags)
    c_iflag - input flags
    c_oflag - output flags
    c_cflag - config flags
  */
  struct termios raw = E.orig_termios; // make copy of original terminal attributes

  /*
    BRKINT - a break condition will cause a terminate signal to be sent to the program
    ICRNL - translates carriage returns inputted by the user into newlines (into 10)
    INPCK - enables parity checking
    ISTRIP - causes the 8th bit of each input byte to be stripped
    IXON - allows stopping of transmission of data to terminal and allow resuming with ctrl+s and ctrl+q
  */
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); // disable above features

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
    ICANON - causes inputs to be read line by line, disabling it makes it read byte by byte
    IEXTEN - terminal waits for another character after ctrl+v
    ISIG - allows process to be terminated with ctrl+c (SIGINT) and suspended with ctrl+z (SIGTSTP)
  */
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // disable above features

  /*
  c_cc - control characters
  VMIN - minimum number of bytes of input needed before read() can return
  VTIME - maximum amount of time to wait before read() returns in (1/10s)
  */
  raw.c_cc[VMIN] = 0;  // set VMIN to 0 so read() returns as soon as there is any input to be read
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
char editorReadKey()
{
  int nread;
  char c;
  while ((nread = read(STDIN_FILENO, &c, 1)) != 1)
  {
    if (nread == -1 && errno != EAGAIN)
      die("read");
  }
  return c;
}

int getWindowSize(int *rows, int *cols) {
  struct winsize ws;

  // 
  if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
  {
    return -1;
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }
}

void initEditor() {
  if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}