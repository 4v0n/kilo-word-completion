#include <stdlib.h>
#include <word_completion.h>

void freeSuggestion(Suggestion *suggestion) {
  free(suggestion->word);
  free(suggestion);
}