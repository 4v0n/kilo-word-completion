#include <prefix_matcher.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <trie.h>
#include <word_completion.h>

#define MAX_LINE_LENGTH 1024

TrieNode root;

List pmGetSuggestions(const char *word) { return; }

bool initPM() {
  root = *getNode();
  FILE *file = fopen(WORDS_PATH, "r");

  if (file == NULL) {
    return NULL;
  }

  char buffer[MAX_LINE_LENGTH];
    while (fgets(buffer, MAX_LINE_LENGTH, file)) {
        char *word = malloc(MAX_LINE_LENGTH * sizeof(char));
        int count;

        // Check if the word and count are successfully parsed
        if (sscanf(buffer, "%[^,],%d", word, &count) == 2) {
            insert(&root, word, count);

            free(word);
        } else {
            // Handle parsing error or free the allocated memory if it's not used
            free(word);
        }
    }

  return true;
}
