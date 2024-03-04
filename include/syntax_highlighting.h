#ifndef SYNTAX_HIGHLIGHTING_H
#define SYNTAX_HIGHLIGHTING_H

#include <data.h>

void editorUpdateSyntax(erow *row);
int editorSyntaxToColour(int hl);

#endif