#include <unistd.h>
#include <terminal.h>

// Draw a column of tildes on the left side of the screen
void editorDrawRows()
{
  int y;
  struct editorConfig *E = getEditorConfig();
  for (y = 0; y < E->screenrows; y++) { // draw 24 rows (placeholder for size of terminal)
    write(STDOUT_FILENO, "~\r\n", 3);
  }
}

// Draws each row of the buffer of text being edited
void editorRefreshScreen()
{
  write(STDOUT_FILENO, "\x1b[2J", 4); // clear terminal
  write(STDERR_FILENO, "\x1b[H", 3); // reposition cursor

  editorDrawRows();

  write(STDOUT_FILENO, "\x1b[H", 3);
}