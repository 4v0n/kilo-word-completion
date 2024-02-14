#include <stdlib.h>
#include <termios.h>
#include <unistd.h> 

struct termios orig_termios;

void disableRawMode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode() {
  tcgetattr(STDIN_FILENO, &orig_termios); // read current terminal attributes into struct
  atexit(disableRawMode); // register disableRawMode() to be called on program exit

  /*
    disable the ECHO feature

    &= bitwise AND and assignment, ~ bitwise NOT

    c_lflag - local flags (miscellaneous flags)
    c_iflag - input flags
    c_oflag - output flags
    c_cflag - config flags
  */
 struct termios raw = orig_termios; // make copy of original terminal attributes
  raw.c_lflag &= ~(ECHO);

  /*
    write the new terminal attributes

    TCSCAFLUSH specifies when to apply the change
      waits for all pending output to be written to the terminal
      also discards any input that hasn't been read
  */
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
  enableRawMode();

  /* 
  Read keypresses from user 

  When running, the terminal is hooked to the standard input
  Keyboard inputs will be read into the c variable
  */
  char c;
  /*
    read() reads 1 byte from the standard input into the variable, c until there are no more bytes to read
    read() returns the number of bytes read, and will return 0 when it reaches the end of a file
    This while loop exits when c is 'q', quitting the program
  */
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');

  return 0;
}