#ifndef WORD_COMPLETION_H
#define WORD_COMPLETION_H

#include <stdbool.h>

typedef struct Suggestion {
  char *word;
  int weight;
} Suggestion;

typedef struct engineConfig {
  bool isActive;
} engineConfig;

void toggleWordCompletionEngine();
void completeWord();
void initWordCompletionEngine();

void freeSuggestion(Suggestion *suggestion);

#endif