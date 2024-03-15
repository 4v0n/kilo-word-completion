/*
  This file contains code adapted from antirez's kilo text editor:
  https://github.com/antirez/kilo and was programmed following Paige Ruten's
  "Build Your Own Text Editor" tutorial:
    https://viewsourcecode.org/snaptoken/kilo/index.html |
    https://github.com/snaptoken/kilo-tutorial?tab=readme-ov-file

  This file prototypes functions that handle syntax highlighting
*/

#ifndef SYNTAX_HIGHLIGHTING_H
#define SYNTAX_HIGHLIGHTING_H

#include <data.h>

void editorUpdateSyntax(erow *row);
int editorSyntaxToColour(int hl);
void editorSelectSyntaxHighlight();

#endif