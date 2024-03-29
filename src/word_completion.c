#include <ctype.h>
#include <editor_operations.h>
#include <output.h>
#include <prefix_matcher.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <terminal.h>
#include <word_completion.h>

#define CTRL_KEY(k) ((k) & 0x1f)

struct engineConfig EC;

struct engineConfig *getEngineConfig() { return &EC; }

char *statusmsg;

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

  if (EC.suggestions.size > 0) {
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
  } else {
    strcat(string, " NO SUGGESTIONS");
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
  size_t rightStringSize = strlen(statusmsg) + strlen(modeString) +
                           2; // +1 for null-terminator, +1 for potential space
  char *rightString = malloc(rightStringSize);
  snprintf(rightString, rightStringSize, "%s %s", statusmsg, modeString);

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

void wordCompletionChooseCompletion(char c) {
  if (c == CTRL_KEY('a')) {
    EC.selection--;
  } else {
    EC.selection++;
  }

  if (EC.selection > (EC.suggestions.size - 1)) {
    EC.selection = 0;
  } else if (EC.selection < 0) {
    EC.selection = EC.suggestions.size - 1;
  }
}

void completeWord() {
  if (!EC.isActive || EC.suggestions.size == 0) {
    return;
  }

  char *word = (char *)getListElement(&EC.suggestions, EC.selection);

  for (int i = 0; i < ((int)strlen(word)); i++) {
    if (i > ((int)strlen(EC.prefix) - 1)) {
      editorInsertChar(word[i]);
    }
  }

  editorInsertChar(' ');

  free(EC.prefix);
  EC.prefix = malloc(1 * sizeof(char));
  EC.prefix[0] = '\0';
}

bool isAlphabetChar(const char c) {
  int val;
  if (isupper(c)) {
    val = tolower(c) - 'a';
  } else {
    val = c - 'a';
  }

  return (val >= 0 && val < 26);
}

char *getWordAtIndex(const char *str, const int index, int rowsize) {
  if (index < 0 || str == NULL || rowsize == 0) {
    return NULL;
  }
  int start = index;

  while (start > 0 && isAlphabetChar(str[start - 1])) {
    start--;
  }

  if ((index - start) > MAX_PREFIX_LENGTH || start == index) {
    return NULL;
  }

  if (index == (int)strlen(str) || !isAlphabetChar(str[index])) {
    int length = index - start;
    char *word = (char *)malloc(length + 1);
    strncpy(word, str + start, length);
    word[length] = '\0';

    return word;
  }
  return NULL;
}

void fillSuggestions(const char *word) {

  List suggestions;
  switch (EC.mode) {
  case PREFIX:
    pmGetSuggestions(word);
    break;
  case FUZZY:
    // execute fuzzy match code
    break;
  }
}

void updateEC() {
  struct editorConfig *E = getEditorConfig();

  if (E->row == NULL) {
    return;
  }

  int pos = E->cx;
  erow row = E->row[E->cy];
  char *rowString = row.chars;

  free(EC.prefix);

  char *word = getWordAtIndex(rowString, pos, row.size);

  if (word != NULL) {
    EC.prefix = word;
    fillSuggestions(word);
  } else {
    EC.prefix = malloc(1 * sizeof(char));
    EC.prefix[0] = '\0';
  }
}

void freeSuggestion(Suggestion *suggestion) {
  free(suggestion->word);
  free(suggestion);
}

bool initMatcher() {
  bool working = false;

  switch (EC.mode) {
  case PREFIX:
    working = initPM();
    break;
  case FUZZY:
    // working = initFM();
    break;
  }

  if (statusmsg != NULL) {
    free(statusmsg);
  }

  if (working) {
    statusmsg = malloc(strlen("\0") + 1);
    strcpy(statusmsg, "\0");
  } else {
    statusmsg = malloc(strlen("Failed to load dataset") + 1);
    strcpy(statusmsg, "Failed to load dataset");
  }
}

void initWordCompletionEngine() {
  EC.isActive = false;
  EC.prefix = malloc(2 * sizeof(char));
  EC.prefix[0] = '\0';

  initList(&EC.suggestions);

  EC.selection = 0;
  EC.mode = PREFIX;

  initMatcher();
}
