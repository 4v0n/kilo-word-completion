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
  raw.c_cc[VMIN] = 0; // set VMIN to 0 so read() returns as soon as there is any input to be read
  raw.c_cc[VTIME] = 1; // set VTIME to 1 to make read return every 100ms (10Hz)

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

  // while the program is running
  while (1) {

    char c = '\0'; // initialise c to null character
    read(STDIN_FILENO, &c, 1); // read 1 byte from standard input into char c

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
    if (c == 'q') break;
  }

  return 0;
}