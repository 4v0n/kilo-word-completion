#include <output.h>
#include <stdlib.h>
#include <word_completion.h>

void completeWord() {
  editorSetStatusMessage("Toggle Word Completion");
}

void freeSuggestion(Suggestion *suggestion) {
  free(suggestion->word);
  free(suggestion);
}