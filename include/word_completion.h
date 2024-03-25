#ifndef WORD_COMPLETION_H
#define WORD_COMPLETION_H

typedef struct Suggestion {
  char *word;
  int weight;
} Suggestion;

void freeSuggestion(Suggestion *suggestion);

#endif