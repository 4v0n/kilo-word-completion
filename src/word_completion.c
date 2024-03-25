#include <output.h>
#include <stdbool.h>
#include <stdlib.h>
#include <terminal.h>
#include <word_completion.h>
#include <string.h>

struct engineConfig EC;

struct engineConfig *getEngineConfig() { return &EC; }

void drawPromptString(struct abuf *ab) {
  struct editorConfig *E = getEditorConfig();

  int len = 0;

  while (len < E->screencols) {
    abAppend(ab, " ", 1);
    len++;
  }
}

void drawWordCompletionPromptRow(struct abuf *ab) {
  struct editorConfig *E = getEditorConfig();
  
  abAppend(ab, "\x1b[46m", 5);
  drawPromptString(ab);
  abAppend(ab, "\x1b[m", 3); // switch back to normal
  abAppend(ab, "\r\n", 2); // newline
}

void toggleWordCompletionEngine() {
  EC.isActive = !EC.isActive;

  if (EC.isActive) {
    getEditorConfig()->screenrows--;
  } else {
    getEditorConfig()->screenrows++;
  }
}

void completeWord() {
  // complete word
}

void freeSuggestion(Suggestion *suggestion) {
  free(suggestion->word);
  free(suggestion);
}

void initWordCompletionEngine() {
  EC.isActive = false;  
  EC.prefix = malloc(sizeof(char));
  EC.suggestions = malloc(sizeof(List));
  initList(EC.suggestions);
  EC.selection = 0;
  EC.mode = PREFIX;
}
