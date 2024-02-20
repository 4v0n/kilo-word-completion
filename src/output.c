#include <unistd.h>

void editorRefreshScreen()
{
  write(STDOUT_FILENO, "\x1b[2J", 4); // clear terminal
  write(STDERR_FILENO, "\x1b[H", 3); // reposition cursor
}