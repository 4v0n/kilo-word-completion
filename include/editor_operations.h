/*
  This file contains code adapted from antirez's kilo text editor:
  https://github.com/antirez/kilo and was programmed following Paige Ruten's
  "Build Your Own Text Editor" tutorial:
    https://viewsourcecode.org/snaptoken/kilo/index.html |
    https://github.com/snaptoken/kilo-tutorial?tab=readme-ov-file

  This file prototypes functions that handle editing operations, such as inserting and deleting characters, and inserting new lines.
  This code relates to the "editor operations" section
*/

#ifndef EDITOR_OPERATIONS_H
#define EDITOR_OPERATIONS_H

void editorInsertChar(int c);
void editorInsertNewLine();
void editorDelChar();

#endif