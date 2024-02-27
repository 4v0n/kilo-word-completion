#include <stdlib.h>
#include <terminal.h>
#include <string.h>

void editorAppendRow(char *s, size_t len) {
  struct editorConfig *E = getEditorConfig();

  E->row = realloc(E->row, sizeof(erow) * (E->numrows + 1));

  int at = E->numrows;
  E->row[at].size = len;
  E->row[at].chars = malloc(len + 1);
  memcpy(E->row[at].chars, s, len);
  E->row[at].chars[len] = '\0';
  E->numrows++;
}