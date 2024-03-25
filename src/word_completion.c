#include <output.h>
#include <stdlib.h>
#include <word_completion.h>
#include <stdbool.h>

struct engineConfig engineConf;

void toggleWordCompletionEngine() {
  engineConf.isActive = !engineConf.isActive;
}

void completeWord() {
  // complete word
}

void freeSuggestion(Suggestion *suggestion) {
  free(suggestion->word);
  free(suggestion);
}

void initWordCompletionEngine() {
  engineConf.isActive = false;
}
