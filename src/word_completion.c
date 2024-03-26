#include <ctype.h>
#include <output.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <terminal.h>
#include <word_completion.h>

struct engineConfig EC;

struct engineConfig *getEngineConfig() { return &EC; }

char *modeToString(int mode) {
  switch (mode) {
  case PREFIX:
    return "| mode: prefix match";
    break;
  case FUZZY:
    return "| mode: fuzzy match";
    break;
  };
  return NULL;
}

void toUpperCase(char *str) {
  int i;
  for (i = 0; str[i] != '\0'; i++) {
    str[i] = toupper(str[i]);
  }
}

int drawSuggestionsString(struct abuf *ab, int maxlen) {
  int len = 0;

  char *string = malloc((maxlen * 2) * sizeof(char));
  string[0] = '\0';

  if (strlen(EC.prefix) == 0) {
    strcpy(string, "(NONE) -> ");
  } else {
    snprintf(string, maxlen * 2, "%s -> ", EC.prefix);
  }

  char *completion = (char *)getListElement(&EC.suggestions, EC.selection);
  strcat(string, completion);
  strcat(string, " | Suggestions: ");

  for (int i = 0; i < EC.suggestions.size; i++) {
    char *s = (char *)getListElement(&EC.suggestions, i);

    if (EC.selection == i) {
      strcat(string, "<");
      strcat(string, s);
      strcat(string, ">");
    } else {
      strcat(string, s);
    }

    strcat(string, " ");
  }

  len = strlen(string);
  if (len > maxlen) {
    len = maxlen;
  }

  abAppend(ab, string, len);

  return len;
}

void drawPromptString(struct abuf *ab) {
  struct editorConfig *E = getEditorConfig();

  char *modeString = modeToString(EC.mode);
  size_t rightStringSize = strlen(WORD_COMPLETE_HELP) + strlen(modeString) +
                           2; // +1 for null-terminator, +1 for potential space
  char *rightString = malloc(rightStringSize);
  snprintf(rightString, rightStringSize, "%s %s", WORD_COMPLETE_HELP,
           modeString);

  int rlen = strlen(rightString);
  int maxlen = E->screencols - rlen;

  int len = drawSuggestionsString(ab, maxlen);

  while (len < E->screencols) {
    if (E->screencols - len == rlen) {
      abAppend(ab, rightString, rlen);
      break;
    } else {
      abAppend(ab, " ", 1); // fill with spaces
      len++;
    }
  }

  free(rightString); // Avoid memory leak
}

void drawWordCompletionPromptRow(struct abuf *ab) {
  abAppend(ab, "\x1b[46m", 5);
  drawPromptString(ab);
  abAppend(ab, "\x1b[m", 3); // switch back to normal
  abAppend(ab, "\r\n", 2);   // newline
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

char *getWordAtIndex(const char *str, const int index) {
  if (index < 0) {
    return NULL;
  }

  int start = index;

  while (start > 0 && str[start - 1] != ' ') {
    start--;
  }

  if (index == (int)strlen(str) || str[index + 1] == '\0' ||
      str[index + 1] == ' ') {
    char *word = malloc(sizeof(char) * (index - start + 2));
    strncpy(word, str + start, index - start + 1);
    word[index - start + 1] = '\0';

    return word;
  }
  return NULL;
}

void updateEC() {
  struct editorConfig *E = getEditorConfig();
  int pos = E->cx;
  erow row = E->row[E->cy];
  char *rowString = row.chars;

  free(EC.prefix);

  char *word = getWordAtIndex(rowString, pos);

  if (word != NULL) {
    EC.prefix = word;
  } else {
    EC.prefix = malloc(1 * sizeof(char));
    EC.prefix[0] = '\0';
  }
}

void freeSuggestion(Suggestion *suggestion) {
  free(suggestion->word);
  free(suggestion);
}

void initWordCompletionEngine() {
  EC.isActive = false;
  EC.prefix = malloc(2 * sizeof(char));
  EC.prefix[0] = '\0';

  initList(&EC.suggestions);

  // temporary
  addElement(&EC.suggestions, "thing1", sizeof("thing1"));
  addElement(&EC.suggestions, "thing2", sizeof("thing2"));
  addElement(&EC.suggestions, "thing3", sizeof("thing3"));
  addElement(&EC.suggestions, "thing4", sizeof("thing4"));
  addElement(&EC.suggestions, "thing5", sizeof("thing5"));

  EC.selection = 0;
  EC.mode = PREFIX;
}
