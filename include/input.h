/*
  This file contains code from antirez's kilo text editor:
  https://github.com/antirez/kilo and was programmed following Paige Ruten's
  "Build Your Own Text Editor" tutorial:
    https://viewsourcecode.org/snaptoken/kilo/index.html |
    https://github.com/snaptoken/kilo-tutorial?tab=readme-ov-file

    This file prototypes functions that handle user input to the editor
*/

#ifndef INPUT_H
#define INPUT_H

char *editorPrompt(char *prompt, void (*callback)(char *, int));
void editorMoveCursor(int key);
void editorProcessKeypress();

#endif