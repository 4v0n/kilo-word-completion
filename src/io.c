#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <output.h>
#include <row_operations.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <terminal.h>
#include <unistd.h>

char *editorRowsToString(int *buflen) {
  struct editorConfig *E = getEditorConfig();

  int totlen = 0;
  int j;
  for (j = 0; j < E->numrows; j++)
    totlen += E->row[j].size + 1;
  *buflen = totlen;

  char *buf = malloc(totlen);
  char *p = buf;
  for (j = 0; j < E->numrows; j++) {
    memcpy(p, E->row[j].chars, E->row[j].size);
    p += E->row[j].size;
    *p = '\n';
    p++;
  }
  return buf;
}

// read file from disk
void editorOpen(char *filename) {
  struct editorConfig *E = getEditorConfig();

  free(E->filename);
  E->filename = strdup(filename);

  FILE *fp = fopen(filename, "r");
  if (!fp)
    die("fopen");

  char *line = NULL;
  size_t linecap = 0;
  ssize_t linelen;
  linelen = getline(&line, &linecap, fp);
  while ((linelen = getline(&line, &linecap, fp)) != -1) {
    while (linelen > 0 &&
           (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
      linelen--;
    editorAppendRow(line, linelen);
  }
  free(line);
  fclose(fp);
}

void editorSave() {
  struct editorConfig *E = getEditorConfig();

  if (E->filename == NULL)
    return;

  int len;
  char *buf = editorRowsToString(&len);

  int fd = open(E->filename, O_RDWR | O_CREAT, 0644);
  if (fd != -1) {
    if (ftruncate(fd, len) != -1) {
      if (write(fd, buf, len) == len) {
        close(fd);
        free(buf);
        editorSetStatusMessage("%d bytes written to disk", len);
        return;
      }
    }
    close(fd);
  }
  free(buf);
  editorSetStatusMessage("Could not save! I/O error: %s", strerror(errno));
}