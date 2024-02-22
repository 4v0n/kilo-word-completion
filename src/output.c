#include <unistd.h>
#include <terminal.h>

// Draw a column of tildes on the left side of the screen
void editorDrawRows()
{
  int y;
  struct editorConfig *E = getEditorConfig();
  for (y = 0; y < E->screenrows; y++)
  {
    write(STDOUT_FILENO, "~", 1);

    if (y < E->screenrows - 1) {
      write(STDOUT_FILENO, "\r\n", 2);
    }
  }
}

// Draws each row of the buffer of text being edited
void editorRefreshScreen()
{
  write(STDOUT_FILENO, "\x1b[2J", 4); // clear terminal
  write(STDERR_FILENO, "\x1b[H", 3);  // reposition cursor

  editorDrawRows();

  write(STDOUT_FILENO, "\x1b[H", 3);
}