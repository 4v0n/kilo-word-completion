#include <terminal.h>
#include <stdlib.h>
#include <unistd.h>

/*** defines ***/
#define CTRL_KEY(k) ((k) & 0x1f) // bitwise AND character with 00011111

/*** functions ***/

// Waits for a keypress and handles it
void editorProcessKeypress()
{
  char c = editorReadKey();

  switch (c)
  {
  case CTRL_KEY('q'): // quit program
    // clear screen and reposition cursor on quit
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    exit(0);
    break;
  }
}