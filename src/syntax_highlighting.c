#include <ctype.h>
#include <data.h>
#include <stdlib.h>
#include <string.h>

void editorUpdateSyntax(erow *row) {
  row->hl = realloc(row->hl, row->rsize);
  memset(row->hl, HL_NORMAL, row->rsize);

  int i;
  for (i = 0; i < row->rsize; i++) {
    if (isdigit(row->render[i])) {
      row->hl[i] = HL_NUMBER;
    }
  }
}

int editorSyntaxToColour(int hl) {
  switch (hl) {
    case HL_NUMBER: return 31;
    default: return 37;
  }
}