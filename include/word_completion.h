#ifndef WORD_COMPLETION_H
#define WORD_COMPLETION_H

#include <data.h>
#include <stdbool.h>
#include <list.h>

enum completionModes { PREFIX = 0, FUZZY };

typedef struct Suggestion {
  char *word;
  int weight;
} Suggestion;

typedef struct engineConfig {
  bool isActive; // state of word completion engine
  char *prefix; // current prefix typed by the user
  List *suggestions; // list of suggestions for the prefix
  int selection; // which suggestion is selected
  int mode; // which completion mode the engine is currently using
} engineConfig;

struct engineConfig *getEngineConfig();

void drawWordCompletionPromptRow(struct abuf *ab);

void toggleWordCompletionEngine();
void completeWord();
void initWordCompletionEngine();

void freeSuggestion(Suggestion *suggestion);

#endif