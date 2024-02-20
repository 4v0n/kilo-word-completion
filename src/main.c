/*** includes ***/
#include <terminal.h>

#include <ctype.h>
#include<stdio.h>
#include <unistd.h>
#include <errno.h>

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f) // bitwise AND character with 00011111

/*** init ***/
int main() {
  enableRawMode();

  // while the program is running
  while (1) {

    char c = '\0'; // initialise c to null character
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) // read 1 byte from standard input into char c
      die("read");

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
    
    if (c == CTRL_KEY('q')) break;

  }

  return 0;
}