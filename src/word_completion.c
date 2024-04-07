/*
  This file handles operations relating to word completion
*/

#include <ctype.h>
#include <editor_operations.h>
#include <language_matcher.h>
#include <levenshtein_matcher.h>
#include <output.h>
#include <prefix_matcher.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <terminal.h>
#include <util.h>
#include <word_completion.h>

#define CTRL_KEY(k) ((k) & 0x1f)

struct engineConfig EC;

struct engineConfig *getEngineConfig() { return &EC; }

char *statusmsg;

// Returns a string communitcating the current completion mode
char *modeToString(int mode) {
  switch (mode) {
  case PREFIX:
    return "| mode: prefix match";
    break;
  case FUZZY:
    return "| mode: fuzzy match";
    break;
  case LANGUAGE:
    return "| mode: language match";
    break;
  };
  return "| mode: undefined";
}

// converts a string to all upper case
void toUpperCase(char *str) {
  int i;
  for (i = 0; str[i] != '\0'; i++) {
    str[i] = toupper(str[i]);
  }
}

// renders the suggestion string and returns the length of the string rendered
int drawSuggestionsString(struct abuf *ab, int maxlen) {
  int len = 0;

  // Create render string
  char *string = malloc((maxlen * 2) * sizeof(char));
  string[0] = '\0';

  // Append prefix as appropriate
  if (strlen(EC.prefix) == 0) {
    strcpy(string, "(NONE) -> ");
  } else {
    snprintf(string, maxlen * 2, "%s -> ", EC.prefix);
  }

  if (EC.suggestions.size > 0) { // if word completions are available
    // Append selected word completion
    char *completion = (char *)getListElement(&EC.suggestions, EC.selection);
    strcat(string, completion);

    // Append rest of available completions
    strcat(string, " | Suggestions: ");
    for (int i = 0; i < EC.suggestions.size; i++) {
      char *s = (char *)getListElement(&EC.suggestions, i);

      if (EC.selection == i) {
        // mark currently selected completion
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

  //  truncate string length if too long
  len = strlen(string);
  if (len > maxlen) {
    len = maxlen;
  }

  // Add string to render buffer
  abAppend(ab, string, len);

  return len;
}

// Renders the word-completion prompt bar
void drawPromptString(struct abuf *ab) {
  struct editorConfig *E = getEditorConfig();

  // Create string for right side of bar
  char *modeString = modeToString(EC.mode);
  size_t rightStringSize = strlen(statusmsg) + strlen(modeString) +
                           2; // +1 for null-terminator, +1 for potential space
  char *rightString = malloc(rightStringSize);
  snprintf(rightString, rightStringSize, "%s %s", statusmsg, modeString);

  // Calc max size of left side of bar
  int rlen = strlen(rightString);
  int maxlen = E->screencols - rlen;

  // Render left side
  int len = drawSuggestionsString(ab, maxlen);

  // Render right side
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

// Handle rendering of word-completion bar
void drawWordCompletionPromptRow(struct abuf *ab) {
  abAppend(ab, "\x1b[46m", 5);
  drawPromptString(ab);
  abAppend(ab, "\x1b[m", 3); // switch back to normal
  abAppend(ab, "\r\n", 2);   // newline
}

// Toggles the word-completion engine
void toggleWordCompletionEngine() {
  EC.isActive = !EC.isActive;

  // Make room for bar
  if (EC.isActive) {
    getEditorConfig()->screenrows--;
  } else {
    getEditorConfig()->screenrows++;
  }
}

// Handle selection of word suggestions
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

// Insert selected word suggestion into editor
void completeWord() {
  if (!EC.isActive || EC.suggestions.size == 0) {
    return; // do nothing if inactive/no suggestions
  }

  char *word = (char *)getListElement(&EC.suggestions, EC.selection);

  // insert characters of word into editor
  for (int i = 0; i < ((int)strlen(word)); i++) {
    if (i > ((int)strlen(EC.prefix) - 1)) {
      editorInsertChar(word[i]);
    }
  }

  editorInsertChar(' ');

  // reset prefix
  free(EC.prefix);
  EC.prefix = malloc(1 * sizeof(char));
  EC.prefix[0] = '\0';

  EC.selection = 0;
  emptyList(&EC.suggestions);
}

char *getWordAtIndex(const char *str, const int index, int rowsize) {
  // exit if pointing to empty string / row
  if (index < 0 || str == NULL || rowsize == 0) {
    return NULL;
  }

  // move backwards until non-alphabet char encountered
  int start = index;
  while (start > 0 && (isAlphabetChar(str[start - 1]) ||
                       ((EC.mode == LANGUAGE) && (str[start - 1] != ' ')))) {
    start--;
  }

  // exit length = 0 or length is greater than max prefix length
  if ((index - start) > MAX_PREFIX_LENGTH || start == index) {
    return NULL;
  }

  // if char after word is end of string or is not an alphabet char
  if (index == (int)strlen(str) || !isAlphabetChar(str[index])) {
    int length = index - start;
    char *word = (char *)malloc(length + 1);
    strncpy(word, str + start, length);
    word[length] = '\0';

    return word;
  }
  return NULL;
}

// Gets suggestions based on prefix and load into EC
void fillSuggestions(const char *word) {
  if (!EC.isActive) {
    return;
  }

  List *suggestions;

  switch (EC.mode) {
  case PREFIX:
    suggestions = pmGetSuggestions(word);
    break;
  case FUZZY:
    suggestions = lmGetSuggestions(word);
    break;
  case LANGUAGE:
    suggestions = langGetSuggestions(word);
  }

  if (suggestions) {
    emptyList(&EC.suggestions);
    EC.suggestions.head = suggestions->head;
    EC.suggestions.size = suggestions->size;
    free(suggestions);
  }
}

// Updates the state of the word completion engine
void updateEC() {
  struct editorConfig *E = getEditorConfig();

  // exit on null row
  if (E->row == NULL) {
    return;
  }

  int pos = E->cx;
  erow row = E->row[E->cy];
  char *rowString = row.chars;

  // free prefix
  free(EC.prefix);

  EC.selection = 0;
  emptyList(&EC.suggestions);

  char *word = getWordAtIndex(rowString, pos, row.size);

  // set new prefix
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

// initialises word completion matcher
bool initMatcher() {
  bool working = false;

  switch (EC.mode) {
  case PREFIX:
    working = initPM();
    break;
  case FUZZY:
    working = initLM();
    break;
  case LANGUAGE:
    working = initLangM();
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

  return working;
}

// initialises word completion engine
void initWordCompletionEngine() {
  EC.isActive = false;
  EC.prefix = malloc(2 * sizeof(char));
  EC.prefix[0] = '\0';

  EC.suggestions = *createList();

  EC.selection = 0;
  EC.mode = PREFIX;

  // initialise to language match
  struct editorConfig *E = getEditorConfig();
  if (E->syntax) {
    EC.mode = LANGUAGE;
  }

  initMatcher();
}

void destroyMatcher() {
  switch (EC.mode) {
  case PREFIX:
    destroyPM();
    break;
  case FUZZY:
    destroyLM();
    break;
  case LANGUAGE:
    destroyLangM();
    break;
  }
}

void toggleCompletionMode() {
  destroyMatcher();

  int newMode = EC.mode + 1;

  if (newMode > (NUM_MODES - 1)) {
    newMode = 0;
  } else if (EC.selection < 0) {
    newMode = NUM_MODES - 1;
  }

  EC.mode = newMode;

  initMatcher();
}