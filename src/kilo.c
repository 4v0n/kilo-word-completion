#include <ctype.h>
#include <stdio.h>
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
    disable terminal features

    &= bitwise AND and assignment, ~ bitwise NOT

    c_lflag - local flags (miscellaneous flags)
    c_iflag - input flags
    c_oflag - output flags
    c_cflag - config flags
  */
  struct termios raw = orig_termios; // make copy of original terminal attributes

  /*
    ICRNL - translates carriage returns inputted by the user into newlines (into 10)
    IXON - allows stopping of transmission of data to terminal and allow resuming with ctrl+s and ctrl+q
  */
  raw.c_iflag &= ~(ICRNL | IXON); // disable above features

  /*
    OPOST - output processing
  */
  raw.c_oflag &= ~(OPOST); // disable above features

  /*
    ECHO - causes each key typed to be printed to terminal
    ICANON - causes inputs to be read line by line, disabling it makes it read byte by byte
    IEXTEN - terminal waits for another character after ctrl+v
    ISIG - allows process to be terminated with ctrl+c (SIGINT) and suspended with ctrl+z (SIGTSTP)
  */
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); // disable above features

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
  while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {

    /*
      iscntrl(c) tests whether c is a control character
        Control characters are nonprintable characters
    */
    if (iscntrl(c)) {
      // printf() can print multiple representations of a byte
      printf("%d\r\n", c); // %d tells it to format the byte as a decimal number (its ASCII code)
    } else {
      printf("%d ('%c')\r\n", c, c); // output as decimal + %c writes the byte directly, as a character
    }
  }

  return 0;
}