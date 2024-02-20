#include <unistd.h>

// Clear the terminal screen
void editorRefreshScreen()
{
  /*
    Write bytes: \x1b [ 2 J
      \x1b - escape character
      [ - [ character - escape + this starts escape sequence
      2 - argument for J - clear the entire screen
      J - erase in display command
  */
  write(STDOUT_FILENO, "\x1b[2J", 4);
}