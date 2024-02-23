#include <unistd.h>
#include <terminal.h>
#include <string.h>
#include <stdlib.h>

/*** append buffer ***/

struct abuf
{
  char *b; // buffer
  int len; // length
};

// empty buffer - constructor for abuf type
#define ABUF_INIT {NULL, 0};

// appends string s to abuf
void abAppend(struct abuf *ab, const char *s, int len)
{
  // give us a block of memory that is the size of the current string + size of string we are appending
  char *new = realloc(ab->b, ab->len + len);

  if (new == NULL)
    return;

  // copy the string s after the end of the current data in the buffer
  memcpy(&new[ab->len], s, len);
  ab->b = new; // update abuf pointer
  ab->len += len; // update length
}

// destructor for abuf
void abFree(struct abuf *ab)
{
  free(ab->b); // deallocate pointer
}

/*** output ***/

// Draw a column of tildes on the left side of the screen
void editorDrawRows(struct abuf *ab)
{
  int y;
  struct editorConfig *E = getEditorConfig();
  for (y = 0; y < E->screenrows; y++)
  {
    abAppend(ab, "~", 1);

    abAppend(ab, "\x1b[K", 3); // clear line
    if (y < E->screenrows - 1)
    {
      abAppend(ab, "\r\n", 2);
    }
  }
}

// Draws each row of the buffer of text being edited
void editorRefreshScreen()
{
  struct abuf ab = ABUF_INIT; // create new append buffer

  abAppend(&ab, "\x1b[?25l", 6); // hide cursor
  abAppend(&ab, "\x1b[H", 3);  // reposition cursor

  editorDrawRows(&ab); 

  abAppend(&ab, "\x1b[H", 3); // reposition cursor
  abAppend(&ab, "\x1b[?25h", 6); // show cursor

  write(STDOUT_FILENO, ab.b, ab.len); // write buffer to standard output
  abFree(&ab); // free buffer
}
