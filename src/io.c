/*
  This file contains code from antirez's kilo text editor:
  https://github.com/antirez/kilo and was programmed following Paige Ruten's
  "Build Your Own Text Editor" tutorial:
    https://viewsourcecode.org/snaptoken/kilo/index.html |
    https://github.com/snaptoken/kilo-tutorial?tab=readme-ov-file

  This file handles file IO operations
*/

// compatibility
#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <input.h>
#include <output.h>
#include <row_operations.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syntax_highlighting.h>
#include <sys/types.h>
#include <terminal.h>
#include <unistd.h>

// Convert the editor rows into a single string
char *editorRowsToString(int *buflen) {
  struct editorConfig *E = getEditorConfig();

  int totlen = 0; // string total length
  int j;
  for (j = 0; j < E->numrows; j++) // calc total length required
    totlen += E->row[j].size + 1; // sum lengths of all rows
  *buflen = totlen; // set the output parameter to the total length

  char *buf = malloc(totlen); // allocate memory for string
  char *p = buf; // pointer to iterate through buf
  for (j = 0; j < E->numrows; j++) {
    memcpy(p, E->row[j].chars, E->row[j].size);
    p += E->row[j].size;
    *p = '\n';
    p++;
  }
  return buf; // return concatonated string
}

// read file from disk
void editorOpen(char *filename) {
  struct editorConfig *E = getEditorConfig();

  // free previous filename
  free(E->filename);
  E->filename = strdup(filename); // duplicate filename

  editorSelectSyntaxHighlight();

  FILE *fp = fopen(filename, "r");
  if (!fp)
    die("fopen");

  char *line = NULL;
  size_t linecap = 0;
  ssize_t linelen;
  while ((linelen = getline(&line, &linecap, fp)) != -1) { 
    // trim newlines or carriage returns
    while (linelen > 0 &&
           (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
      linelen--;
    editorInsertRow(E->numrows, line, linelen); // insert line into editor
  }
  free(line); // free getline buffer
  fclose(fp);
  E->dirty = 0;
}

// Save editor contents to file on disk
void editorSave() {
  struct editorConfig *E = getEditorConfig();

  if (E->filename == NULL) { // prompt file name if needed
    E->filename = editorPrompt("Save as: %s (ESC to cancel)", NULL);
    if (E->filename == NULL) {
      editorSetStatusMessage("Save aborted");
      return;
    }
    editorSelectSyntaxHighlight();
  }

  int len;
  char *buf = editorRowsToString(&len);

  // open or create file
  int fd = open(E->filename, O_RDWR | O_CREAT, 0644);
  if (fd != -1) {
    if (ftruncate(fd, len) != -1) {
      if (write(fd, buf, len) == len) {
        close(fd);
        free(buf);
        E->dirty = 0;
        editorSetStatusMessage("%d bytes written to disk", len);
        return;
      }
    }
    close(fd);
  }
  free(buf);
  editorSetStatusMessage("Could not save! I/O error: %s", strerror(errno));
}